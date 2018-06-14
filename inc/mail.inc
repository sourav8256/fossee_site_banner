<?php


function fossee_site_banner_mail($key, &$message, $params)
{

  global $user;
  switch($key)
  {
    case 'banner_created':
      $message['to'] = $params['to'];
      $message['bcc'] = 'site_admin_mail_goes_here';
      $message['subject'] = t('New Banner is Created');
      $message['body'][] = t('Hello,');
      $message['body'][] = t('New banner "'.$params['banner_name'].'" has been created at fossee.in.');
      break;

    case 'banner_expiry_reminder':
      $message['to'] = $params['to'];
      $message['bcc'] = 'site_admin_mail_goes_here';
      $message['subject'] = t('New Banner is Created');
      $message['body'][] = t('Hello,');
      $message['body'][] = t('The banner "'.$params['banners'].'" is valid only for 7 or less number of days please take necessary action.');
      break;
  }
}