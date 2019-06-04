</div> <!-- close the article grid_9 -->

<div id="sidebar" class="grid_3">
	<ul class="sidebar_list">		
		<?php if (!function_exists('dynamic_sidebar') || !dynamic_sidebar()) : ?>	
		<?php get_links_list('id'); ?>
		<?php endif; ?>
	</ul>
</div> <!-- close the sidebar grid_3 -->

</div> <!-- close the container_12 div -->
