<?php

/* 
 * This function is used to create new banners
 * 
 */



function new_edit_banner_form($form, &$form_state,$arg_id) {

  $result = db_select('fossee_banner_details','n') // for fetching the banner filename
  ->fields('n')
  ->range(0,1)
  ->condition('n.id',$arg_id,'=')
  ->execute();

  $object = $result->fetchAll();



//   $form['upload_item'] = array(
//     '#type' => 'file',
//     '#title' => 'Upload File',
//     '#description' => "Allowed file types: [".variable_get('fossee_site_banner_allowed_file_types', NULL)."] and Max. file size: ".variable_get('fossee_site_banner_max_file_size', NULL)/(1024*1024)." MBs",
//   );

  $form['banner_name'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => t('Banner Name : <br/>'),
    '#size' => 10,
    '#prefix' => '<br/><br/>',
    '#default_value' => $object[0]->banner_name,
    '#suffix' => '',
  );

  $form['banner_href'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => t('Banner Redirect Url : <br/>'),
    '#description' => t('The url to open when the banner is clicked'),
    '#size' => 10,
    '#default_value' => $object[0]->banner_href,
    '#prefix' => '<br/><br/>',
    '#suffix' => '<br/><br/>',
  );

  $form['date'] = array(
    '#prefix'=>t('<br/><br/>'),
    '#date_format' => 'Y-m-d',
    '#date_year_range' => '0:+1',
    '#type' => 'date_popup',
    '#weight'=>0,
    '#default_value' => date('Y-m-d', $object[0]->timestamp),
    '#datepicker_options' => array('minDate' => 0),
    '#required' => TRUE,
  );

  $form['banner_id'] = array( // hidden field that stores the banner id received from the argument
    '#type' => 'hidden',
    '#value' => $arg_id,
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
function new_edit_banner_form_validate($form,&$form_state){
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

function new_edit_banner_form_submit($form, &$form_state) {

  $id = $form_state['values']['banner_id'];
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

    try {
      $db_update = db_update('fossee_banner_details')
        ->fields(array(
          'banner_name' => $banner_name,
          'timestamp' => $timestamp,
          'last_updated' => $created_timestamp,
          'banner_href' => $banner_href,
        ))
        ->condition('id',$id,'=')
        ->execute();
    } catch (Exception $e) {
      drupal_set_message("Sorry Banner has not been created due to some unknown database error!".$e);
      return;
    }


    if($db_update) {

      //drupal_set_message("File Successfully Uploaded : " . $file->filename);
      drupal_set_message('Banner Name : ' . $banner_name);
      drupal_set_message('End Date : ' . date('Y-m-d H:i:s', $end_date));
      drupal_set_message('Banner href : ' . $banner_href);

      drupal_goto("../banner-settings/".$id,array('external' => TRUE));

    } else {
      drupal_set_message("Sorry Banner Is Not Created For Some Reason!");
    }


}
