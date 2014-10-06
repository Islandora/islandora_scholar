<?php
/**
 * @file
 * This is the template file for the object page for thesis objects.
 */
?>
<div class="islandora-object-image">
  <?php if (isset($variables['preview']) && isset($variables['pdf'])): ?>
    <a href="<?php print $variables['pdf']; ?>">
      <img src="<?php print $variables['preview']; ?>"/>
    </a>
  <?php endif; ?>
</div>

<?php if (isset($variables['metadata'])): ?>
  <div class="thesis_metadata">
    <?php print $variables['metadata']; ?>
  </div>
<?php endif; ?>
