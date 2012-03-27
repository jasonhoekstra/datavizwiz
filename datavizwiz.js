/* 
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */


function wrapPrepareForFlot(container, parent_section, widthToHeight, call_block) {
        var parent_originally_hidden = $(parent_section).css("display") == "none";
        if (parent_originally_hidden) {
          $(parent_section).show();
        }
        $(container).width( $(parent_section).width() );
        $(container).height( $(parent_section).width() * widthToHeight );
        if (parent_originally_hidden) {
          parent_section.addClass("ui-helper-hidden-accessible");
        }

        call_block(container);

        if (parent_originally_hidden) {
          $(parent_section).removeClass("ui-helper-hidden-accessible");
          $(parent_section).hide();
        }
    }