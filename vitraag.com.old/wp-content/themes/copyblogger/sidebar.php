<div id="sidebar">
	<ul class="sidebar_list">
		<li class="widget">
			<h2>Search</h2>
			<?php include (TEMPLATEPATH . '/searchform.php'); ?>
		</li>
		<?php if (!function_exists('dynamic_sidebar') || !dynamic_sidebar()) : ?>
		<li class="widget">
			<h2>Latest Blog Entries</h2>
			<ul>
				<?php query_posts('showposts=10'); ?>
				<?php if (have_posts()) : while (have_posts()) : the_post(); ?>
				<li><a href="<?php the_permalink() ?>"><?php the_title() ?></a></li>
				<?php endwhile; endif; ?>
<li><a href="http://technorati.com/claim/5gythtvaj2" rel="me">Technorati Profile</a>
			</ul>
		</li>
		<li class="widget">
			<h2>Categories</h2>
			<ul>
				<?php wp_list_categories('title_li=0'); ?>
			</ul>
		</li>
                <li class="widget">
                        <h2>My Books</h2>
<script language="javascript" type="text/javascript" src="http://www.librarything.com/jswidget.php?reporton=vaibhavb&show=recent&header=1&num=10&covers=small&text=title&tag=alltags&amazonassoc=vitraag-20&css=1&style=2&version=1">
</script>
               </li>
		<?php get_links_list('id'); ?>
		<?php endif; ?>
	</ul>
</div>