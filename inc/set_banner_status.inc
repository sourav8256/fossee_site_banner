<?php

/* 
 * This function receives the ajax call to set a banner active the banner is identified using the argument $id sent with the call
 * 
 */

function set_banner_active($id) {

	$num_updated = db_update('banner_details') 
	->fields(array(
		'status' => 'active',
		'status_bool' => 1,
    'last_updated' => time(),
	))
	->condition('id', $id, '=')
	->execute();

	if($num_updated){
		$result = 'success';
	} else {
    # code...
    $result = 'failed';
	}

	drupal_json_output(array('status' => 0, 'data' => $id, 'result' => $result));
}

/* 
* This function is used to set banners inactive
*/

function set_banner_inactive($id) {

  /*
   * set status field to inactive and status_bool to 0 */
	$num_updated = db_update('banner_details')
	->fields(array(
		'status' => 'inactive',
		'status_bool' => 0,
    'last_updated' => time(),
	))
	->condition('id', $id, '=')
	->execute();


	if($num_updated){
		$result = 'success';
	} else {
    # code...
		$result = 'failed';
	}

	drupal_json_output(array('status' => 0, 'data' => $id,'result' => $result));
}
