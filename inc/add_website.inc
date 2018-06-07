<?php
/**
 * Created by PhpStorm.
 * User: sourav
 * Date: 1/6/18
 * Time: 2:46 PM
 */


function add_website_form($form, &$form_state) {

  $form['website_name'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => t('Website Domain : <br/>'),
    '#size' => 10,
    '#description' => t(''),
    //'#prefix' => '<br/><br/>',
    //'#suffix' => '<br/><br/>',
  );

  $form['upload_submit'] = array(
    '#type' => 'submit',
    '#value' => 'Submit',
    '#prefix' => '',
  );

  $path =  "http://".$_SERVER['SERVER_NAME'] . base_path();

  return $form;

}

function add_website_form_validate($form,&$form_state){
  $site_name = $form_state['values']['website_name'];

  if (filter_var($site_name, FILTER_VALIDATE_URL)) {

  } else {

    if (filter_var("http://".$site_name, FILTER_VALIDATE_URL)) {

    } else {

      form_set_error("website_name","Please Give A Valid Domain Name !");
    }

  }
}


/*
 * Submit the new_banner form and save details to database
 *
 */

function add_website_form_submit($form, &$form_state) {

  $site_name = $form_state['values']['website_name'];
  $parsed_url = parse_url($site_name);

  if($parsed_url['host'] != NULL){
    $domain = $parsed_url['host'];
  } elseif($parsed_url['path'] != NULL) {
    $domain = $parsed_url['path'];
  }


  try {
    $db_insert = db_insert('fossee_website_index')
      ->fields(array(
        'site_name' => $domain,
      ))
      ->execute();
    drupal_set_message("Domain added : ".$domain);
  } catch (PDOException $e) {
    if($e->errorInfo[1] === 1062){
      drupal_set_message("Error 1062 : The Website Already Exists, Please Enter A Different Website!");
    }
  } catch (Exception $e){
    drupal_set_message("Unknown Database Exception : Website Is Not Created");

  }

}
