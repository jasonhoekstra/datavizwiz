$(document).ready(function(){
  initTabs();
});
 
function initTabs() {
  $('#dvwTabs a').bind('click',function(e) {
    e.preventDefault();
    var thref = $(this).attr("href").replace(/#/, '');
    $('#dvwTabs a').removeClass('dvwActive');
    $(this).addClass('dvwActive');
    $('#dvwTabContent div.dvwContent').removeClass('dvwActive');
    $('#'+thref).addClass('dvwActive');
  });
}
