<?php

function datavizwiz_api($nid, $operation) {
  $node = node_load($nid);
  $rows = _datavizwiz_get_data_array($nid);

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
    case 'd3json':
      // Backing away from this, refactor for OpenLayers chrolopeth
      // _datavizwiz_return_d3_json($rows);
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

function _datavizwiz_get_latlon_columns($nid) {
  
}

function _datavizwiz_return_csv($rows) {
  //drupal_set_header("Content-type: text/csv");
  drupal_set_header("Cache-Control: no-store, no-cache");
  //drupal_set_header('Content-Disposition: attachment; filename="data.csv"');
  
  $colnames = _datavizwiz_get_column_names($rows[0]);
  echo _datavizwiz_export_csv($rows, $colnames);
}

function _datavizwiz_return_geojson($rows, $latloncols) {
  drupal_set_header("Content-type: application/json");
  drupal_set_header("Cache-Control: no-store, no-cache");
  
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

        $feature->id = $row['ImporterID'];
        $properties = new stdClass();
        $properties->popupContent = 'Test';
        $feature->properties = $properties;

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
  // Drupal function: drupal_json does not return valid JSON
  echo json_encode($rows);
}

function _datavizwiz_return_xml($rows) {
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
  //print_r($xml);
}

function _datavizwiz_return_d3_json($rows) {
  
  echo '{"01":18.9,"02":11,"04":17.6,"05":18.7,"06":15.8,"08":13.2,"09":10.1,"10":11.9,"11":18.8,"12":16.5,"13":18,"15":11.1,"16":15.8,"17":13.8,"18":15.3,"19":12.5,"20":13.5,"21":18.9,"22":18.8,"23":13.1,"24":9.9,"25":11.4,"26":16.7,"27":11.5,"28":22.4,"29":15.3,"30":15.2,"31":12.6,"32":14.8,"33":8.6,"34":10.2,"35":19.8,"36":15,"37":17.4,"38":12.5,"39":15.8,"40":16.8,"41":15.8,"42":13.4,"44":14.1,"45":18.1,"46":14.6,"47":17.8,"48":17.9,"49":13.3,"50":12.4,"51":11.1,"53":13.5,"54":18.2,"55":13.2,"56":11.4}';
  
  //return "d3_json";
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