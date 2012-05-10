<?php 
  drupal_add_js(drupal_get_path('module', 'datavizwiz') . '/scripts/datavizwiz.js');
  drupal_add_js(drupal_get_path('module', 'datavizwiz') . '/libraries/excanvas.compiled.js');
  drupal_add_css(drupal_get_path('module', 'datavizwiz') . '/datavizwiz.css');
  
  if (isset($_GET['detail']) && strlen($_GET['detail'])) {
    $detail_page = TRUE;
  } else {
    $detail_page = FALSE;
  }
  
  
?>
<div id="node-<?php print $node->nid; ?>" class="datavizwiz node<?php if ($sticky) { print ' sticky'; } ?><?php if (!$status) { print ' node-unpublished'; } ?> clear-block">

<?php print $picture ?>

<?php if (!$page): ?>
  <h2><a href="<?php print $node_url ?>" title="<?php print $title ?>"><?php print $title ?></a></h2>
<?php endif; ?>

  <?php if (!$teaser) { // Don't display the data visualization if a teaser view. '?>
  <?php if (!$detail_page) { ?>
    <div class="content">
      <?php print $content ?>
    </div>
  <?php } ?>

  <div class="dvwArea" id="dvw-datadisplay">
    <?php if ($detail_page) { ?>
      <div class="dvwArea" id="dvw-detail">
        <?php print theme('datavizwiz_detaildisplay', $node); ?>
      </div> 
    <?php } else { ?>
      <div class="dvwArea" id="dvw-summarypanes">
        <?php print theme('datavizwiz_summarypanes', $node); ?>
      </div>  

      <div class="dvwArea" id="dvw-datafilters">
        <?php print theme('datavizwiz_datafilters', $node); ?>
        <div style="clear: both" />
      </div>

      <div class="dvwArea" id="dvw-opendatalinks">
        <?php print theme('datavizwiz_opendatalinks', $node); ?>
      </div>

      <div class="dvwArea" id="dvw-datatable">
        <?php print theme('datavizwiz_datatable', $node); ?>
      </div>
    <?php } // detail ?>
  </div>
  <?php } // teaser ?>
    
  <?php print $links; ?>

  <div class="meta">
  <?php if ($submitted): ?>
    <p><span class="submitted">Last updated: <?php print $submitted ?></span></p>
  <?php endif; ?>
    
  <?php if ($terms): ?>
    <div class="terms terms-inline"><?php print $terms ?></div>
  <?php endif;?>
  </div>

</div>