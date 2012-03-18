<?php


function datavizwiz_edit_one($nid) {
  $headers = array( 'Field', 'Display', 'Sortable', 'Visible Summary','Visible Detail', 'Visible Data', 'Summary Order', 'Detail Order');
  $tablerows = _datavizwiz_get_dataset_metadata($nid);

  $headerrows = array();
  
  if (count($tablerows) > 0) {
    foreach ($tablerows[0] as $key => $value) {
      if ($key != 'fid' && $key != 'nid' && $key != NULL) {
        array_push($headerrows, $key);
      }   
    }
}

  $displayrows = array();
  
  foreach ($tablerows as $tablerow) {
    if ($tablerow['field_name'] != 'ImporterID') {
      $displayrow = array();
      
      foreach ($headerrows as $headerrow) {
        $value = $tablerow[$headerrow];
        if ($value == 1) { $checked = 'CHECKED'; } else { $checked = ''; }
        switch ($headerrow) {
          case 'sortable' :
          case 'visible_summary' :
          case 'visible_detail' :
          case 'visible_data' :
              $html = '<input type="checkbox" name="'. $headerrow .'" '.$checked.'/>';
            break;
          default :
            $html = $value;
        }
        
        array_push($displayrow, $html);
        //dsm($tablerow[$headerrow]);
      }
      array_push($displayrows, $displayrow);      
    }
  }
  
  $output = theme_table($headers, $displayrows);
  $output .= theme('pager', NULL, 0);
  
  return $output;
  
}

function datavizwiz_edit_two($nid) {
  return 'hello!';
}