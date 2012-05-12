$(document).ready(function(){
  initTabs();
});
 
function initTabs() {
  $('#dvwTabs a').bind('click',function(e) {
    e.preventDefault();
    var thref = $(this).attr("id").replace(/#/, '');
    thref = 'info' + thref;
    $('#dvwTabs a').removeClass('dvwActive');
    $(this).addClass('dvwActive');
    //$('#dvwTabContent div.dvwContent').removeClass('dvwActive');
    $('#dvwTabContent div.dvwContent').css('left', '-20000px');
    //$('#'+thref).addClass('dvwActive');
        $('#'+thref).css('left', '0px');
    //alert(thref);
  });
}
