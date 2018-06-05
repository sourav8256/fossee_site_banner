function deleteBanner() {
    var id = Drupal.settings.banner_manager.id;
    var baseUrl = Drupal.settings.banner_manager.base_url;
    console.log("the log is "+baseUrl);
    var txt;
    if (confirm("Banner Once Deleted Is Not Recoverable, do you still want to delete it permanently?")) {
        txt = "You pressed OK!";
        $.get("http://localhost/fossee/fossee_drupal/banner_manager/delete_banner/"+id+'/arg'+Math.random()).success(function(data){

            console.log("result is "+data.result);

            if(data.result === 'success'){
                window.location = baseUrl+"banner_manager/banners";
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
    Drupal.settings.banner_manager = {
        attach: function (context, settings) {
            alert(settings.MODULENAME.testvar);
        }
    };

})(jQuery);*/
