<?php


/* 
 * This function shows all the available banners and can be used to set the banners active/inactive and links to the page to select the sites the banner will be available in
 * 
 */

function banner_manager_banners(){

  //$dir = "public://site_banners"; // The directory in which banner files are kept
  $output = NULL;

  setFrontend(); // this function renders the fromtend like rendering javascript and css style

  $recent_time = 1*24*60*60; // D * H * MIN * SEC

  $result_recent = db_select('banner_details','n')
    ->fields('n',array('id','file_name','status'))
    //->range(0,20)
    //->condition(db_or()->condition('n.created_timestamp',time()-$recent_time,'>')->condition('n.last_updated',time()-$recent_time))
    ->condition('n.last_updated',time()-$recent_time,">")
    ->execute(); // returns the banners from the banner_details table

  $result_active = db_select('banner_details','n')
    ->fields('n',array('id','file_name','status'))
    //->range(0,20)
    ->condition('n.status_bool',1,'=')
    //->condition('n.created_timestamp',time()-$recent_time,'<=')
    ->execute(); // returns the banners from the banner_details table

  $result_inactive = db_select('banner_details','n')
    ->fields('n',array('id','file_name','status'))
    //->range(0,20)
    ->condition('n.status_bool',1,'!=')
    //->condition('n.created_timestamp',time()-$recent_time,'<=')
    ->execute(); // returns the banners from the banner_details table
  // Open a directory, and read its contents

  $output .= generateTable($result_recent,"Recent");
  $output .= generateTable($result_active,"Active");
  $output .= generateTable($result_inactive,"Inactive");

  return $output;

}


/* 
 * This function returns all the available banner files from database table banner_details 
 * used in function banner_manager_banners
 */

function getFilesArray(){
  $result = db_select('banner_details','n')
    ->fields('n',array('id','file_name','status'))
    ->range(0,20)
    //->condition('n.status_bool',1,'=')
    ->execute();

  return $result;
}


function generateTable($results,$table_title){

  $dir = variable_get('banner_manager_banner_directory', "not found");
  $path =  site_banners_path(); // path to the folder containing the banner files
  $realpath = file_create_url($dir); //
  $settings_form_link = $path =  "http://".$_SERVER['SERVER_NAME'] . base_path() . 'banner_manager/banner_settings/';
  $col_length = 3; // The number of columns in the table in which banners are displayed
  $i=0;
  $banners = array();
  $banner = new stdClass();

  foreach ($results as $result) { // this foreach statement loops throught the result containing banner_details returned from the banner_details table in database
    # code...

    $banner->file_name = $result->file_name;
    $banner->status = $result->status;
    $banner->id = $result->id;

    $banners[($i/$col_length)][($i%$col_length)] = $banner;
    unset($banner);
    $i++;
  }

  for($j=0;$j<sizeof($banners);$j++){ // loops through rows of banner images for checking necessary conditons and rendered accordingly

    unset($col); // col array contains the html code for displaing the banner images

    for($k=0;$k<sizeof($banners[$j]);$k++){ // loops through columns of $j row of the banners array
      unset($banner);
      $banner = $banners[$j][$k];

      if($banners[$j]!=NULL){ // necessary for rows which contains less than 3 banners
        if($banner->status == 'active'){
          $status_pointer = "border: 10px solid orange"; // this style adds the orange border for displaying the active banners
        } else {
          $status_pointer = "";
        }


        $col[$k] = '
			<div style = "height : 170px; width : 200px;">
			<img class="imgStyle" onclick="imageClicked(this); return false;" id="'.$banner->id.'" src="'.$realpath."/".$banner->file_name.'" style="height:60%;width: 100%;'.$status_pointer.'">
			<button type="button" style = "width : 100%" class="btn" onclick="window.open(\''.$settings_form_link.$banner->id.'\',\'_self\')">Banner Settings</button>
			</div>
			';

      } else {
        # code...
        $col[$k] = ""; // if there is no image this element of col is left empty
      }
    }


    /* the html code for displaying banners is put into rows array which will be later used for rendering the table*/
    $rows[] = array(

      $col[0],
      $col[1],
      $col[2],

    );
  }

  /* checks that the $rows is not empty and renders the table */
  if (!empty($rows)) {

    try {
      $header = array($table_title,"","");
      $output = theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => 'sort-table')));
      $output .= theme('pager');
    } catch (Exception $e) {
    }


  } else {

    //$output .= t("No results found.");

  }

  return $output;

}


/* 
 * This function sets javascript and css code
 * 
 */

function setFrontend(){


  $res = getFilesArray();

  $style = ".imgStyle
	{
		border : 10px solid orange;
	}";

  $jquery = 'https://ajax.googleapis.com/ajax/libs/jquery/2.1.3/jquery.min.js';
  drupal_add_js($jquery);
  $bannersArray = "";

  foreach ($res as $resu) {
    # code...
    $bannersArray =$bannersArray."'".$resu->id."'"." : "."'".$resu->status."'"." , ";
  }

  /* The following is the javascript to handle ajax calls to set the banners active and inactive */

  $js="
  //var bannersArray = new Array();
	var bannersArray = {".$bannersArray."};
	console.log(bannersArray);

	(function($) {
		$.fn.myJavascriptFunction = function(data) {
			alert(data);
		};
	})(jQuery);

	function imageClicked(param){
		console.log(bannersArray);
		changeCaller(param);
	}

	function changeCaller(param) {

		if(bannersArray[param.id] == \"active\"){
			setBannerInctiveAjaxCall(param);
		} else {
			setBannerActiveAjaxCall(param);
		}

	}

	function setBannerActiveAjaxCall(param){
		console.log(new Date().getTime());
		console.log(\"http://localhost/fossee/fossee_drupal/banner_manager/set_banner_active/\"+param.id+'/arg'+Math.random());
		$.get(\"http://localhost/fossee/fossee_drupal/banner_manager/set_banner_active/\"+param.id+'/arg'+Math.random()).success(function(data){ 
			if(data.result == 'success'){
				setBannerSelected(data.data);
			}

		});

	}

	function setBannerSelected(id){
	  onSuccess();
		//document.getElementById(id).style.border = \"10px solid orange\";
		//bannersArray[id] = \"active\";
	}

	function setBannerInctiveAjaxCall(param){
		console.log(new Date().getTime());
		console.log(\"http://localhost/fossee/fossee_drupal/banner_manager/set_banner_inactive/\"+param.id+'/arg'+Math.random());
		$.get(\"http://localhost/fossee/fossee_drupal/banner_manager/set_banner_inactive/\"+param.id+'/arg'+Math.random()).success(function(data){ 
			if(data.result == 'success'){
				setBannerUnselected(data.data);
			}
		});

	}

	function setBannerUnselected(id){
	  onSuccess();
		//document.getElementById(id).style.border = \"0px solid orange\";
		//bannersArray[id] = \"inactive\";
	}

  function onSuccess(){
    console.log('Onsuccess called');
    location.reload();
  }

	function changeImage(param) {
		if(bannersArray[param.id] == \"active\"){
			param.style.border = \"0px solid orange\";
			bannersArray[param.id] = \"inactive\";
		} else {
			param.style.border = \"10px solid orange\";
			bannersArray[param.id] = \"active\";
		}
	}

	";

  drupal_add_js($js, 'inline', 'header');
}




function site_banners_path() { // returns the file location as a directory path
  $path =  $_SERVER['DOCUMENT_ROOT'] . base_path() . 'site_banners';
  return $path;
}

function site_banners_url() { // returns the file path as an url
  //$path =  $_SERVER['DOCUMENT_ROOT'] . base_path() . 'site_banners';
  $path =  "http://".$_SERVER['SERVER_NAME'] . base_path() . 'site_banners/';
  return $path;
}
