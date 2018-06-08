function deleteBanner() {
    var id = Drupal.settings.fossee_site_banner.id;
    var baseUrl = Drupal.settings.fossee_site_banner.base_url;
    console.log("the log is "+baseUrl);
    var txt;
    if (confirm("Banner Once Deleted Is Not Recoverable, do you still want to delete it permanently?")) {
        txt = "You pressed OK!";
        $.get(baseUrl+"fossee-site-banner/delete-banner/"+id+'/arg'+Math.random()).success(function(data){

            console.log("result is "+data.result);

            if(data.result === 'success'){
                window.location = baseUrl+"fossee-site-banner/banners";
                console.log("result success");
            }

            return false;

        });

    } else {
        txt = "You pressed Cancel!";
    }

    return false;
}


/*
(function($) {
    Drupal.settings.fossee_site_banner = {
        attach: function (context, settings) {
            alert(settings.MODULENAME.testvar);
        }
    };

})(jQuery);*/
