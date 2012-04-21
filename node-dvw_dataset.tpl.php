<?php
if (isset($_GET['json_d3'])) {
  //drupal_set_header();
  //echo 'true';
  //echo $node->nid;
  //dsm($node); 
  //exit;
  //ddebug_backtrace();
  die;
} else {
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

  <div class="datadisplay">
    <?php print theme('datavizwiz_displaydata', $node); ?>
  </div>

  <?php print $links; ?>

  <div class="meta">
  <?php if ($submitted): ?>
    <p><span class="submitted">Last updated: <?php print $submitted ?></span></p>
  <?php endif; ?>

</div>
<?php } ?>