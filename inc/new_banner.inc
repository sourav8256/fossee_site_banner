<?php

/* 
 * This function is used to create new banners
 * 
 */

function new_banner_form($form, &$form_state) {

  module_load_include('inc','fossee_site_banner','inc/mail.inc');

  $form['upload_item'] = array(
    '#type' => 'file',
    '#title' => 'Upload File',
    '#description' => "Allowed file types: [".variable_get('fossee_site_banner_allowed_file_types', NULL)."] and Max. file size: ".variable_get('fossee_site_banner_max_file_size', NULL)/(1024*1024)." MBs",
  );

  $form['banner_name'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => t('Banner Name : <br/>'),
    '#size' => 10,
    '#prefix' => '<br/><br/>',
    '#suffix' => '',
  );

  $form['banner_href'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => t('Banner Redirect Url : <br/>'),
    '#description' => t('The url to open when the banner is clicked'),
    '#size' => 10,
    '#prefix' => '<br/><br/>',
    '#suffix' => '<br/><br/>',
  );

  $form['date'] = array(
    '#prefix'=>t('<br/><br/>'),
    '#date_format' => 'Y-m-d',
    '#date_year_range' => '0:+1',
    '#type' => 'date_popup',
    '#weight'=>0,
    '#datepicker_options' => array('minDate' => 0),
    '#required' => TRUE,
  );


  $form['upload_submit'] = array(
    '#type' => 'submit',
    '#value' => 'Submit',
    '#prefix' => '',
    '#suffix' => '<br/><br/>',
  );


  $path =  "http://".$_SERVER['SERVER_NAME'] . base_path();

  $form['html'] = array(

    '#type' => 'button',
    '#value' => t('See Available Banners'),
    '#attributes' => array(
      'onclick' => 'window.open(\''.$path.'fossee-site-banner/banners'.'\',"_self"); return false;',
    ),

  );


  return $form;

}


/*
 * validating new banner form
 *
 */
function new_banner_form_validate($form,&$form_state){
  $end_date = strtotime($form_state['values']['date']." 23:59:59"); // the date when the banner has to stop displaying
  $timestamp = strtotime($form_state['values']['date']); // converts time string from form to timestamp to be saved in database
  if(time()>$timestamp){ //checking if banner validity is before today's date
    form_set_error("date","Sorry! you can't give a date before today's date!");
  }


  /*
   * Validating banner redirect url using inbuilt php function filter_var
   * */
  $url = $form_state['values']['banner_href'];
  if (filter_var($url, FILTER_VALIDATE_URL)) {

  } else {
    form_set_error("banner_href","The Banner Redirect Url is invalid please give a valid url including http:// or https://");
  }

}


/* 
 * Submit the new_banner form and save details to database
 * 
 */

function new_banner_form_submit($form, &$form_state) {

  module_load_include('inc','fossee_site_banner','inc/mail');
  global $user;

  $allowed_extensions = variable_get('fossee_site_banner_allowed_file_types', NULL);
  //$allowed_extensions = str_replace(","," ",$allowed_extensions);

  $validators = array(
    'file_validate_size' => array(variable_get('fossee_site_banner_max_file_size', NULL)), // size restriction for banner image size
    'file_validate_extensions' => explode(",",$allowed_extensions) // allowed extensions of banner image
  );

  $location = variable_get('fossee_site_banner_banner_directory', "not found");
  $real_path = drupal_realpath($location);
  $banner_name = $form_state['values']['banner_name']; // the name of the banner file
  $banner_href = $form_state['values']['banner_href'];
  //$end_date = $form_state['values']['datetime'];
  $end_date = strtotime($form_state['values']['date']." 23:59:59"); // the date when the banner has to stop displaying
  //$banner_name = $form_state['values']['banner_name']; // The name of the banner as set by the user
  //$filename = $file->filename;
  $timestamp = strtotime($form_state['values']['date']); // converts time string from form to timestamp to be saved in database
  $created_timestamp = time();

  /*saving uploaded file to a temporary location*/
  $file = file_save_upload('upload_item', $validators, "temporary://"); // check validation from the $validators array and saves to a database with location

  if($file) { // check if the file passed the validation and was uploaded

    $new_filename = generateNewFilename($file->filename,$created_timestamp);
    $file->status = FILE_STATUS_PERMANENT; // sets file status to permanent so it isn't removed while clearing cache

    /* moving the file in temporary to the desired location and rename it to a filename_created_timestamp format */
    if(!rename(drupal_realpath("temporary://")."/".$file->filename, $real_path."/".$new_filename)){
      form_set_error("upload_item","Banner Not Created : File Renaming Failed!");
      return;
    }
    $file->filename = $new_filename;
    $file->uri = $location."/".$new_filename;
    $file = file_save($file); // save file permanently


    try {
      $db_insert = db_insert('fossee_banner_details')
        ->fields(array(
          'banner_name' => $banner_name,
          'file_name' => $file->filename,
          'timestamp' => $timestamp,
          'created_timestamp' => $created_timestamp,
          'last_updated' => $created_timestamp,
          'banner_href' => $banner_href,
        ))
        ->execute();
    } catch (Exception $e) {
      drupal_set_message("Sorry Banner has not been created due to some unknown database error!".$e);
      return;
    }


    if($db_insert) {

      drupal_set_message("File Successfully Uploaded : " . $file->filename);
      drupal_set_message('Banner Name : ' . $banner_name);
      drupal_set_message('End Date : ' . date('Y-m-d H:i:s', $end_date));

      $params['banner_name'] = $banner_name;
      $params['to'] = variable_get('fossee_site_banner_banner_admin',NULL);
      drupal_mail('fossee_site_banner','banner_created',$user->mail,language_default(),$params);

      drupal_goto("banners",array('external' => TRUE));

    } else {
      drupal_set_message("Sorry Banner Is Not Created For Some Reason!");
    }

  } else {
    //drupal_set_message("Sorry File Could Not Be Uploaded Please Select The Correct File Type And Size!");
  }

}


function generateNewFilename($filename,$timestamp){
  $extension = pathinfo($filename,PATHINFO_EXTENSION);
  $new_filename = $filename."_".$timestamp.".".$extension;
  return $new_filename;
}
