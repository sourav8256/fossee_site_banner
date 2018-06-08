<?php
/**
 * Created by PhpStorm.
 * User: sourav
 * Date: 7/6/18
 * Time: 5:02 PM
 */


function delete_website($site_code){

  $op_result = TRUE;


  /* deleting database entry in fossee_banner_details table */
 $delete_row = db_delete('fossee_website_index')
    ->condition('site_code', $site_code, '=')
    ->execute();


  /* checking if database row was deleted */
  if(!$delete_row){
    $op_result = FALSE;
  }


  if($op_result){
    $result = 'success';
  } else {
    # code...
    $result = 'failed';
  }

  drupal_goto("../add-website",array('external' => TRUE));

}