<?php


function datavizwiz_tab_manage_fields($nid) {
  $headers = array( 'Field', 'Display', 'Sortable', 'Visible Summary','Visible Detail', 'Visible Data');
  $tablerows = _datavizwiz_get_dataset_metadata($nid);

  $headerrows = array();
  
  if (count($tablerows) > 0) {
    foreach ($tablerows[0] as $key => $value) {
      if ($key != 'fid' && $key != 'nid' && $key != NULL && $key != 'order_summary' & $key != 'order_detail') {
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
  
//  $output .= '<table>';
//  $output .= drupal_get_form('datavizwiz_fields_form');
//  $output .= '</table>';
  
  
  //$form = drupal_get_form('datavizwiz_fields_form');
  $form = datavizwiz_fields_form($form);
  $output .= datavizwiz_theme_summary_form($form);

  
  //$output .= theme_table($headers, $displayrows);
  //$output .= theme('pager', NULL, 0);
  
  return $output;
  
}

function datavizwiz_fields_form() {
  $nid = 14;


  $headers = array( 'Field', 'Display', 'Sortable', 'Visible Summary','Visible Detail', 'Visible Data');
  $tablerows = _datavizwiz_get_dataset_metadata($nid);

  $headerrows = array();
  
  if (count($tablerows) > 0) {
    foreach ($tablerows[0] as $key => $value) {
      if ($key != 'fid' && $key != 'nid' && $key != NULL && $key != 'order_summary' & $key != 'order_detail') {
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
              $form['id' . $tablerow['fid']][$headerrow] =  array(
                '#type' => 'checkbox',
                  '#value' => $checked,
              );
            break;
          default :
              $form['id' . $tablerow['fid']][$headerrow] =  array(
                  '#value' => $value,
              );
            $html = $value;
        }
        
        
        array_push($displayrow, $html);
      }
      array_push($displayrows, $displayrow);      
    }
  }
  
    $form['submit'] = array(
  '#type' => 'submit',
  '#value' => t('Save'),
);

  dsm($form);
  
  return $form;
}

function datavizwiz_theme_summary_form($form) {
	$rows = array();
	foreach (element_children($form) as $key) {
		$row = array();
		if (isset($form[$key]['name'])) {
 
			$status = drupal_render($form['featured'][$key]);
			$row[] = array('data' => $status, 'class' => 'checkbox');
 
			$row[] = ''. drupal_render($form[$key]['name']) .'';
			$row[] = array('data' => drupal_render($form[$key]['category']));
			$row[] = array('data' => drupal_render($form[$key]['discount']));
			$row[] = array('data' => drupal_render($form[$key]['createdon']));
 
			$rows[] = $row;
		}
	}
 
	 // Individual table headers.
	$header = array();
	$header[] = array('data' => t('Featured'), 'class' => 'checkbox');
	$header[] = t('Name');
	$header[] = t('Category');
	$header[] = t('Discount');
	$header[] = t('Created on');
  
    $headers = array( 'Field', 'Display', 'Sortable', 'Visible Summary','Visible Detail', 'Visible Data');
 
	$output = theme('table', $headers, $rows);
	$output .= drupal_render($form);
	//return $output;
}



function datavizwiz_edit_two($nid) {
  return 'hello!';
}