<?php
/**
 * Created by PhpStorm.
 * User: sourav
 * Date: 1/6/18
 * Time: 10:59 PM
 */

function delete_banner($id){

  $op_result = TRUE;

  /* fetching the filename of the banner file */
  $file_name = db_select('fossee_banner_details','n')
    ->fields('n',array('file_name'))
    ->range(0,1)
    ->condition('n.id',$id,'=')
    ->execute()
    ->fetchCol();


  /* deleting database entry in fossee_banner_details table */
  $delete_row = db_delete('fossee_banner_details')
    ->condition('id', $id, '=')
    ->execute();


  /* checking if database row was deleted */
  if(!$delete_row){
    $op_result = FALSE;
  }

  /* checking if file was deleted*/
  if(!file_unmanaged_delete(variable_get('fossee_site_banner_banner_directory', "not found")."/".$file_name[0])){
    $op_result = FALSE;
  }

  if($op_result){
    $result = 'success';
  } else {
    # code...
    $result = 'failed';
  }


  drupal_json_output(array('status' => 0, 'data' => $id, 'result' => $result));
}