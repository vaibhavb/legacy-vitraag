<?php
/*
Template Name: Archives
*/
?>

<?php get_header(); ?>
		
	<div id="content_box">

		<div id="content" class="page">
		
			<h1>Browse the Archives...</h1>
			<div class="entry">
				<p><strong>Monthly archives:</strong><p>
				<ul>
					<?php wp_get_archives('type=monthly'); ?>
				</ul>
				<p><strong>Topical archives:</strong></p>
				<ul>
					<?php 
		wp_list_categories('show_count=0&title_li=&hide_empty=0&exclude=1'); ?>
				</ul>
			</div>
			
		</div> <!-- closes content -->
</div> <!-- closes content_box --

<?php get_sidebar(); ?>
				
<?php get_footer(); ?>
