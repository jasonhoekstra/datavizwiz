<?php

function datavizwiz_api($nid, $operation) {
  
  $object = new stdClass();
  $object->id = 1;
  $object->type = "object";
  $object->description = "sample object";
  
  return json_encode($object);
}