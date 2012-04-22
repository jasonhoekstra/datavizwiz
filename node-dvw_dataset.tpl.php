<?php 
  drupal_add_js(drupal_get_path('module', 'datavizwiz') . '/datavizwiz.js');
  drupal_add_css(drupal_get_path('module', 'datavizwiz') . '/datavizwiz.css');

?>
<div id="node-<?php print $node->nid; ?>" class="datavizwiz node<?php if ($sticky) { print ' sticky'; } ?><?php if (!$status) { print ' node-unpublished'; } ?> clear-block">

<?php print $picture ?>

<?php if (!$page): ?>
  <h2><a href="<?php print $node_url ?>" title="<?php print $title ?>"><?php print $title ?></a></h2>
<?php endif; ?>

  <?php if ($terms): ?>
    <div class="terms terms-inline"><?php print $terms ?></div>
  <?php endif;?>
  </div>

  <div class="content">
    <?php print $content ?>
  </div>

  <div class="dvwarea datadisplay">
    <?php //print theme('datavizwiz_datadisplay', $node); ?>
    
    <div class="dvwarea summarypanes">
      <?php print theme('datavizwiz_summarypanes', $node); ?>
    </div>  

    <div class="dvwarea datafilters">
      <?php print theme('datavizwiz_datafilters', $node); ?>
    </div>
    
    <div class="dvwarea opendatalinks">
      <?php print theme('datavizwiz_opendatalinks', $node); ?>
    </div>
    
    <div class="dvwarea datatable">
      <?php print theme('datavizwiz_datatable', $node); ?>
    </div>
  </div>

  <?php print $links; ?>

  <div class="meta">
  <?php if ($submitted): ?>
    <p><span class="submitted">Last updated: <?php print $submitted ?></span></p>
  <?php endif; ?>

</div>