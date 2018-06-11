<?php
/**
 * Created by PhpStorm.
 * User: sourav
 * Date: 1/6/18
 * Time: 7:21 PM
 */


function banner_settings_form($form,&$form_state){

  $form['file_size'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum file size allowed for upload (in MBs)'),
    '#description' => t('The maximum file size allowed for upload'),
    '#element_validate' => array("element_validate_number"),
    '#size' => 50,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#default_value' => variable_get('fossee_site_banner_max_file_size', '')/(1024 * 1024),
  );
  $form['extensions'] = array(
    '#type' => 'textfield',
    '#title' => t('Allowed image file extensions'),
    '#description' => t('A space separated list of source file extensions that are permitted to be uploaded on the server'),
    '#size' => 50,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#default_value' => variable_get('fossee_site_banner_allowed_file_types', ''),
  );
  $form['banner_dir'] = array(
    '#type' => 'textfield',
    '#title' => t('Banner Directory'),
    '#description' => t('Location where all banner images will be stored'),
    '#size' => 50,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#default_value' => variable_get('fossee_site_banner_banner_directory', ''),
  );

  $res_banner_dir = db_select('fossee_new.fossee_site_banner_variables','n')
          ->fields('n',array('value'))
          ->range(0,1)
          ->condition('n.name','banner_dir','=')
          ->execute()
          ->fetchCol();
  $banner_folder = $res_banner_dir[0];

  $form['banner_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Banner Directory Url(without trailing "/")'),
    '#description' => t('Url to the directory where banners are stored(without trailing "/")'),
    '#size' => 50,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#default_value' => $banner_folder,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit')
  );
  return $form;
}

function banner_settings_form_validate($form, &$form_state){
  return;
}

function banner_settings_form_submit($form, &$form_state){
  $banner_dir = $form_state['values']['banner_dir'];
  $banner_url = $form_state['values']['banner_url'];

  variable_set('fossee_site_banner_max_file_size', $form_state['values']['file_size'] * 1024 * 1024);
  variable_set('fossee_site_banner_allowed_file_types', $form_state['values']['extensions']);
  drupal_set_message(t('Settings updated'), 'status');
  if (!is_dir($banner_dir)) {
    if (drupal_mkdir($banner_dir, NULL, TRUE, NULL)) {
      variable_set('fossee_site_banner_banner_directory', $form_state['values']['banner_dir']);
    } else {
      drupal_set_message(t("Failure : could not create directory"), "error");
    }
  } else {
    variable_set('fossee_site_banner_banner_directory', $form_state['values']['banner_dir']);
  }

  $db_update = db_update('fossee_site_banner_variables')
  ->fields(array(
    'value' => $banner_url,
  ))
  ->condition("name","banner_dir","=")
  ->execute();

  if($db_update == 0){
    $db_insert = db_insert('fossee_site_banner_variables')
        ->fields(array(
          'name' => 'banner_dir',
          'value' => $banner_url,
        ))
        ->execute();
  }


}

