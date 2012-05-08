<?php

function datavizwiz_api($nid, $operation) {
  $node = node_load($nid);
  $rows = _datavizwiz_get_data_array($nid);
  // Since we're returning user controlled open data, allow this to be accessed
  // by cross-site domains.
  drupal_set_header('Access-Control-Allow-Origin: *');
  drupal_set_header("Cache-Control: no-store, no-cache");

  switch (strtolower($operation)) {
    case 'csv':
      if ($node->field_dvw_download_csv[0]['value'] == 1) {
        _datavizwiz_return_csv($rows);
      }
      break;
    case 'geojson':
      // Skipping the check for geojson as its required for maps and only exposes lat/lons and title
      $latloncols = _datavizwiz_get_latlon_cols($nid);
      _datavizwiz_return_geojson($rows, $latloncols);
      break;
    case 'json':
      if ($node->field_dvw_download_json[0]['value'] == 1) {
        _datavizwiz_return_json($rows);
      }
      break;
    case 'xml':
      if ($node->field_dvw_download_xml[0]['value'] == 1) {
        _datavizwiz_return_xml($rows);
      }
      break;
    default:
      echo 'Invalid operation';
      break;
  }
}

function _datavizwiz_get_data_array($nid) {
  if (!is_numeric($nid)) {
    die;
  } // Check for numeric node ID, stop if not
  $node = node_load($nid);
  $datatable = $node->field_dvw_datatable[0]['value'];

  //All of our tables from the Importer module are prefixed with import_, if not here, stop processing
  if (substr($datatable, 0, 7) != 'import_') {
    die;
  }

  $query = sprintf("
      SELECT field_name 
      FROM {dvw_field_meta} 
      WHERE nid=%s AND visible_data=1", $node->nid);
  $colrows = _datavizwiz_get_rowarray(db_query($query));
  
  $filters = array();
  
  if (count($colrows)) {
    foreach($colrows as $colrow) {
      $colnames .= $colrow['field_name'] . ',';
    }
    
    $colnames = substr($colnames, 0, strlen($colnames) - 1);

    $whereclause = _datavizwiz_get_where_clause_by_node_id($nid);
    $result = db_query(sprintf('SELECT ImporterID as RowID,%s FROM %s %s', $colnames, $datatable, $whereclause));
 
    while ($row = db_fetch_array($result)) {
      $rows[] = $row;
    }

    return $rows;
  } else { 
    echo 'No columns are defined for data export.';
    die;
  }
}

function _datavizwiz_return_csv($rows) {
  drupal_set_header("Content-type: text/csv");
  drupal_set_header('Content-Disposition: attachment; filename="data.csv"');
  
  $colnames = _datavizwiz_get_column_names($rows[0]);
  echo _datavizwiz_export_csv($rows, $colnames);
}

function _datavizwiz_return_geojson($rows, $latloncols) {
  drupal_set_header("Content-type: application/json");
  
  // If this request is coming from an OpenLayers summary view, we'll get a
  // ID of the summary in the form of ol_id in the querystring.  If it's
  // present, let's find the title field for the geoJSON return.
  $summary_viewid = check_plain($_GET['ol_id']);
  $title_field = '';
  if (is_numeric($summary_viewid)) {
    $sql = 'SELECT options FROM {dvw_summary_views} WHERE svid=%s';
    $result = db_query($sql, $summary_viewid);
    $record = db_fetch_object($result);
    if ($record) {
      $options = json_decode($record->options);
      $title_field = $options->title_field;
    }
  }
  
  
  if (count($latloncols) != 2) {
    $return = "Latitude and longitude is not defined for this data set.";
  } else {
    $latcol = '';
    $loncol = '';
    
    foreach($latloncols as $col) {
      if ($col['flag'] == DVW_FLAG_LATITUDE) {
        $latcol = $col['field_name'];
      }
      if ($col['flag'] == DVW_FLAG_LONGITUDE) {
        $loncol = $col['field_name'];
      }
    }
    
    if ($latcol == '' || $loncol == '') {
      $return = "Latitude and longitude is not defined for this data set.";
    } else {
      foreach($rows as $row) {
        $feature = new stdClass();
        $feature->type = 'Feature';

        $geometry = new stdClass();
        $geometry->type = 'Point';
        $geometry->coordinates = array((float) $row[$loncol], (float) $row[$latcol]);
        $feature->geometry = $geometry;

        $feature->id = $row['RowID'];
        $properties = new stdClass();
        
        if (strlen($title_field)) {
          if ($row[$title_field] != null) {
            $properties->popupContent = $row[$title_field];
          } else {
            $properties->popupContent = '';
          }
          $feature->properties = $properties;
        }
        
        $geojson[] = $feature;
      }

      $return = new stdClass();
      $return->type = 'FeatureCollection';
      $return->features = $geojson;
    }
  }
  
  echo json_encode($return);
}

function _datavizwiz_return_json($rows) {
  drupal_set_header("Content-type: application/json");

  // Drupal function: drupal_json does not return valid JSON
  echo json_encode($rows);
}

function _datavizwiz_return_xml($rows) {
  drupal_set_header("Content-type: application/xml");

  $colnames = _datavizwiz_get_column_names($rows[0]);
  $xml = simplexml_load_string("<dataset></dataset>");

  foreach($rows as $row) {
    $data = $xml->addChild('data');
    foreach($colnames as $colname) {
      $element = $data->addChild($colname, $row[$colname]);
    }
  }
  
  drupal_set_header("Content-Type:text/xml");
  echo $xml->asXML();
}

function _datavizwiz_export_csv($data, $col_headers = array(), $return_string = false) {
  $stream = ($return_string) ? fopen('php://temp/maxmemory', 'w+') : fopen('php://output', 'w');

  if (!empty($col_headers)) {
    fputcsv($stream, $col_headers);
  }

  foreach ($data as $record) {
    fputcsv($stream, $record);
  }

  if ($return_string) {
    rewind($stream);
    $retVal = stream_get_contents($stream);
    fclose($stream);
    return $retVal;
  }
  else {
    fclose($stream);
  }
}

function _datavizwiz_get_column_names($array) {
  foreach(array_keys($array) as $colname) {
    $colnames[] = $colname;
  }
  
  return $colnames;
}