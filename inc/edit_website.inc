<?php

function edit_website_form($form,&$form_state,$arg_site_code){

  $site_name = db_select('fossee_website_index','n')
  ->fields('n',array('site_name'))
  ->range(0,1)
  ->condition('n.site_code',$arg_site_code,'=')
  ->execute()
  ->fetchCol(); // fetches first column of teh result

  $form['site_name'] = array(
    '#type' => 'textfield',
    //'#required' => TRUE,
    '#title' => t('Edit Domain : <br/>'),
    //'#size' => 10,
    '#default_value' => $site_name[0],
    '#description' => t('Please enter the correct website'),
  );

  $form['site_code'] = array( // hidden field that stores the banner id received from the argument
    '#type' => 'hidden',
    '#value' => $arg_site_code,
  );

  $form['submit'] = array( // submit
    '#type' => 'submit',
    '#value' => 'Submit Form',
    '#prefix' => '<br/><br/>',
    '#name' => 'update_website',
  );
  
  $form['goback'] = array( // submit
    '#type' => 'submit',
    '#value' => 'Go Back',
    '#prefix' => '<br/><br/>',
    '#name' => 'go_back',
  );

  return $form; 
}

function edit_website_form_submit($form,&$form_state,$arg_site_code){
  $site_code = $form_state['values']['site_code'];
  $site_name = $form_state['values']['site_name'];

  if($form_state['triggering_element']['#name'] == "update_website"){
    update_website($site_code,$site_name); 
    //drupal_goto("../add-website",array('external' => TRUE));
  } elseif($form_state['triggering_element']['#name'] == "go_back") {
    drupal_goto("../add-website",array('external' => TRUE));
  }

  drupal_goto("../add-website",array('external' => TRUE));

}

function update_website($site_code,$site_name){
 
  $parsed_url = parse_url($site_name);

  if ($parsed_url['host'] != NULL) {
    $domain = $parsed_url['host'];
  } elseif ($parsed_url['path'] != NULL) {
    $domain = $parsed_url['path'];
  }


  try {

    $db_insert = db_update('fossee_website_index')
      ->fields(array(
        'site_name' => $site_name,
      ))
      ->condition("site_code",$site_code,"=")
      ->execute();
    drupal_set_message(t('Domain Updated To : ' . $site_name),'status');
  } catch (PDOException $e) {
    if ($e->errorInfo[1] === 1062) {
      drupal_set_message("Error 1062 : The Website ".$site_name." Already Exists, Please Enter A Different Website!");
    }
  } catch (Exception $e) {
    drupal_set_message("Unknown Database Exception : Website Is Not Created");

  }
}