
<?php

/* 
 * This function renders the page to select the sites in which teh particular banner will appear
 * 
 */

function banner_settings_form($form,&$form_state,$arg_banner_id){


  $base_url =  "http://".$_SERVER['SERVER_NAME'] . base_path();

  drupal_add_js(array('fossee_site_banner' => array('id' => $arg_banner_id, 'base_url' => $base_url)), array('type' => 'setting'));
  drupal_add_js(drupal_get_path("module","fossee_site_banner")."/js/main.js");
  //$banner_dir = "public://site_banners/";
  $banner_dir = variable_get('fossee_site_banner_banner_directory', "not found");

  $realpath = file_create_url($banner_dir);


  $result = getSitesList(); // fetches the list of sites from fossee_website_index table

  /* getting the list of allowed sites saved as json */

  $allowed_site_json = db_select('fossee_banner_details','n')
    ->fields('n',array('allowed_sites'))
    ->range(0,1)
    ->condition('n.id',$arg_banner_id,'=')
    ->execute()
    ->fetchCol(); // fetches first column of teh result

  $banner_filename = db_select('fossee_banner_details','n') // for fetching the banner filename
  ->fields('n',array('file_name'))
    ->range(0,1)
    ->condition('n.id',$arg_banner_id,'=')
    ->execute()
    ->fetchCol();

  $allowed_sites = json_decode($allowed_site_json[0],true); // receives a json string containing allowed site codes and decodes it into an array

  foreach ($result as $res) {
    # code...
    $sites[$res->site_code] = t($res->site_name); // gets names of the sites from $result to be displayed in the form
  }


  $form['banner'] = array(
    "#prefix" => '<img src="'.$realpath.'/'.$banner_filename[0].'">',
  );


  $form['sites'] = array( // renders chckboxes with corresponding list of sites that the user can select to include in the list of allowed sites
    '#title' => t('Allowed Sites'),
    '#type' => 'checkboxes',
    '#description' => t('Select the sites you would like to display this  banner on.'),
    '#options' => $sites,
    '#default_value' => $allowed_sites,
  );

  $form['add_new_website'] = array(
    '#prefix' => "<a href='../add_website'>Add New Websites</a>",
  );

  $form['banner_id'] = array( // hidden field that stores the banner id received from the argument
    '#type' => 'hidden',
    '#value' => $arg_banner_id,
  );

  $form['submit'] = array( // submit
    '#type' => 'submit',
    '#value' => 'Submit Form',
    '#prefix' => '<br/><br/>'
  );

  $form['delete'] = array(
    '#prefix' => "<button onclick='deleteBanner(); return false;' >Delete Banner</button>&nbsp;&nbsp;",
  );

  $form['back'] = array(
    '#prefix' => str_replace("url",$base_url."/fossee_site_banner/banners","<button onclick='window.open(\"url\",\"_self\"); return false;' >Go Back</button>"),
  );

  return $form;


}

/*
 * This function returns the lists of indexed sites from the table website index for use in function banner_settings_form
 *
 */

function getSitesList(){
  $result = db_select('fossee_website_index','n')
    ->fields('n',array('site_code','site_name'))
    ->range(0,50)
    //->condition('n.uid',$uid,'=')
    ->execute();

  return $result;

}


/*
 * This function currently submits the list of sites the banners  are allowed to be displayed in and saves it to the fossee_banner_details database as json
 *
 */

function banner_settings_form_submit($form,&$form_state){

  $banner_id = $form_state['values']['banner_id']; // gets the banner id from the form

  $allowed_site_json = json_encode(array_filter($form_state['values']['sites'])); // converts array into json


  $num_updated = db_update('fossee_banner_details')
    ->fields(array(
      'allowed_sites' => $allowed_site_json, // field in fossee_banner_details table containing list of allowed sites as json string
    ))
    ->condition('id', $banner_id, '=') // matches the banner id
    ->execute();


  $form_state['redirect'] = $base_url."/fossee_site_banner/banners";
}