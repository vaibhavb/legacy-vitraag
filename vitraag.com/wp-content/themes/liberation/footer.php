

<div id="footer-container">
<div class="container_12">
	
	<div class="credits grid_9">
		<p>&copy; <?php bloginfo('name'); ?> &#8212;
		Powered by <a href="http://www.wordpress.org">Wordpress</a> | 
		<a href="http://www.vitraag.com">Liberation</a> theme design by 
		<a href="http://www.vitraag.com">Vaibhav Bhandari </a></p>
	</div>
	
	<p id="back-top" class="grid_3 align-right"><a href="#">Back to top</a></p>

	<p class="grid_12 debug">
		<?php echo get_num_queries();?>&nbsp;queries.<?php timer_stop(1);?>&nbsp;seconds.
	</p>		
	
	<!-- WP generated footer -->
	<?php wp_footer(); ?>
	<!-- END generated footer -->

</div> <!-- container_12 -->
</div> <!-- footer-container -->

</body>
</html>
