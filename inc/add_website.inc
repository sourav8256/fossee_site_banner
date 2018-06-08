<?php
/**
 * Created by PhpStorm.
 * User: sourav
 * Date: 1/6/18
 * Time: 2:46 PM
 */


function add_website_form($form, &$form_state) {
  $path =  "http://".$_SERVER['SERVER_NAME'] . base_path();

  $form['websites_table'] = array(
    '#markup' => generateWebsitesTable(),
  );

  /*  $form['website_name'] = array(
      '#type' => 'textfield',
      //'#required' => TRUE,
      '#title' => t('Website Domain : <br/>'),
      '#size' => 10,
      '#description' => t(''),
      //'#prefix' => '<br/><br/>',
      //'#suffix' => '<br/><br/>',
    );*/

  $form['ajax_container'] = array(
    '#type'   => 'fieldset',
    '#title'  => t(""),
    //'#weight' => $form['body']['#weight'] + 1,
    '#prefix' => '<div id="ajax_wrapper">',
    '#suffix' => '</div>'
  );

  //$num_checkboxes = !empty($form_state['values']['add_field']) ? $form_state['values']['add_field'] : 1;
  //$num = ++$form_state['values']['add_field'];
  $form_state['n_items'] = !empty($form_state['n_items']) ? $form_state['n_items'] : 1;
  $num = $form_state['n_items'];
  //drupal_set_message("num is ".$num);

  for ($i=0;$i<$num;$i++) {
    $form['ajax_container']['id'.$i] = array(
      '#type' => 'textfield',
      '#tree' => TRUE,
      //'#required' => TRUE,
      '#title' => t('Website Domain : <br/>'),
      '#size' => 10,
      '#description' => t(''),
    );
  }

  $form['ajax_container']['add_field'] = array(
    '#type'   => 'submit',
    '#value'  => t('Add new field'),
    '#submit' => array('my_module_ajax_add_field'),
    '#ajax'   => array(
      'callback'=> 'my_module_ajax_callback',
      'wrapper' => 'ajax_wrapper',
      'effect'  => 'fade',
    ),
    '#name' => 'website_field_add_button',
  );

  $form['ajax_container']['remove_field'] = array(
    '#type'   => 'submit',
    '#value'  => t('Remove a field'),
    '#submit' => array('my_module_ajax_remove_field'),
    '#ajax'   => array(
      'callback'=> 'my_module_ajax_callback',
      'wrapper' => 'ajax_wrapper',
      'effect'  => 'fade',
    ),
    '#name' => 'website_field_remove_button',
  );

  $form['upload_submit'] = array(
    '#type' => 'submit',
    '#value' => 'Submit',
  );

  $form['all_banners'] = array(
    '#markup' => '<button onclick="window.open(\'banners\',\'_self\'); return false;">All Banners</button>',
  );


  return $form;

}


function my_module_ajax_add_field($form, &$form_state) {
  $form_state['n_items']++;
  $form_state['rebuild'] = TRUE;
}

function my_module_ajax_remove_field($form, &$form_state) {
  $form_state['n_items']--;
  $form_state['rebuild'] = TRUE;
}

function my_module_ajax_callback($form, $form_state) {
  return $form['ajax_container'];
}

function generateWebsitesTable(){
  $base_url = "http://".$_SERVER['SERVER_NAME'] . base_path();
  $banners = db_select('fossee_website_index','n') // for fetching the banner filename
  ->fields('n',array('site_name','site_code'))
    ->execute();

  foreach ($banners as $banner){
    $col1 = $banner->site_name;
    $col2 = "<a href='".$base_url."fossee-site-banner/delete-website/".$banner->site_code."'>Delete</a>";
    $rows[] = array($col1,$col2);
    //$rows[] = array("key ","value ");
  }

  $header = array("Available Websites","");
  $output = theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => 'sort-table')));
  $output .= theme('pager');

  return $output;

}


function add_website_form_validate($form,&$form_state){


  //drupal_set_message("message");
  //drupal_set_message("the field is ".$form_state['values']['id0']." and n_items is ");

  $no_of_fields = $form_state['n_items'];
  //drupal_set_message("number of fields ".$no_of_fields);
  for($i=0;$i<$no_of_fields;$i++){
    $site_name = $form_state['values']['id'.$i];

    if(($form_state['triggering_element']['#name'] == 'website_field_add_button') || ($form_state['triggering_element']['#name'] == 'website_field_remove_button')){continue;};
    //if(empty($site_name)){continue;}

    if (filter_var($site_name, FILTER_VALIDATE_URL)) {

    } else {

      if (filter_var("http://".$site_name, FILTER_VALIDATE_URL)) {

      } else {

        form_set_error("id".$i,"Please Give A Valid Domain Name ! ".$site_name." is invalid");
      }

    }
  }

}


/*
 * Submit the new_banner form and save details to database
 *
 */

function add_website_form_submit($form, &$form_state) {

  $no_of_fields = $form_state['n_items'];
  //drupal_set_message("number of fields ".$no_of_fields);
  for($i=0;$i<$no_of_fields;$i++) {

    $site_name = $form_state['values']['id'.$i];
    $parsed_url = parse_url($site_name);

    if ($parsed_url['host'] != NULL) {
      $domain = $parsed_url['host'];
    } elseif ($parsed_url['path'] != NULL) {
      $domain = $parsed_url['path'];
    }


    try {
      $db_insert = db_insert('fossee_website_index')
        ->fields(array(
          'site_name' => $domain,
        ))
        ->execute();
      drupal_set_message("Domain added : " . $domain);
    } catch (PDOException $e) {
      if ($e->errorInfo[1] === 1062) {
        drupal_set_message("Error 1062 : The Website ".$site_name." Already Exists, Please Enter A Different Website!");
      }
    } catch (Exception $e) {
      drupal_set_message("Unknown Database Exception : Website Is Not Created");

    }
  }

}
