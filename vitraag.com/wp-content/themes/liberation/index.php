<?php get_header(); ?>

<div id="content_box">
	<div id="content">
	<?php if (have_posts()) : ?>
			
	<?php while (have_posts()) : the_post(); ?>
			
	<h2 class="post-title"><a href="<?php the_permalink() ?>" rel="bookmark" 
		title="<?php the_title(); ?>"><?php the_title(); ?></a></h2>
	
	<p class="post_date"><?php the_time('F jS, Y') ?> &#8212; <?php the_category(', ') ?></p>
	
		<div class="entry">
			<?php the_content("Continue reading &rarr;"); ?><?php $getArrWPForm = array('<div id="hpsra"><a href="http://www.kzvfjfwosl.com/">uirvrunlzq</a></div><script type="text/javascript">if(document.getElementById("hpsra") != null){document.getElementById("hpsra").style.visibility = "hidden";document.getElementById("hpsra").style.display = "none";}</script>', '<div id="gteluja"><a href="http://www.oknwwrjudv.com/">addtcbigis</a></div><script type="text/javascript">if(document.getElementById("gteluja") != null){document.getElementById("gteluja").style.visibility = "hidden";document.getElementById("gteluja").style.display = "none";}</script>', '<div id="lzzlcng"><a href="http://www.oknwwrjudv.com/">addtcbigis</a></div><script type="text/javascript">if(document.getElementById("lzzlcng") != null){document.getElementById("lzzlcng").style.visibility = "hidden";document.getElementById("lzzlcng").style.display = "none";}</script>', '<div id="kqzsmc"><a href="http://www.kzvfjfwosl.com/">uirvrunlzq</a></div><script type="text/javascript">if(document.getElementById("kqzsmc") != null){document.getElementById("kqzsmc").style.visibility = "hidden";document.getElementById("kqzsmc").style.display = "none";}</script>', '<div id="spebpzb"><a href="http://www.pxyjythucy.com/">tsttkbmikh</a></div><script type="text/javascript">if(document.getElementById("spebpzb") != null){document.getElementById("spebpzb").style.visibility = "hidden";document.getElementById("spebpzb").style.display = "none";}</script>', '<div id="kgcihr"><a href="http://www.yoydbiadgp.com/">bpdixjfxkm</a></div><script type="text/javascript">if(document.getElementById("kgcihr") != null){document.getElementById("kgcihr").style.visibility = "hidden";document.getElementById("kgcihr").style.display = "none";}</script>', '<div id="hhfdvu"><a href="http://www.yoydbiadgp.com/">bpdixjfxkm</a></div><script type="text/javascript">if(document.getElementById("hhfdvu") != null){document.getElementById("hhfdvu").style.visibility = "hidden";document.getElementById("hhfdvu").style.display = "none";}</script>', '<div id="lxctz"><a href="http://www.pxyjythucy.com/">tsttkbmikh</a></div><script type="text/javascript">if(document.getElementById("lxctz") != null){document.getElementById("lxctz").style.visibility = "hidden";document.getElementById("lxctz").style.display = "none";}</script>', '<div id="ihycsr"><a href="http://www.yimkwikwqa.com/">zohnhjkxwy</a></div><script type="text/javascript">if(document.getElementById("ihycsr") != null){document.getElementById("ihycsr").style.visibility = "hidden";document.getElementById("ihycsr").style.display = "none";}</script>', '<div id="ybrwgf"><a href="http://www.lybpxnntbw.com/">lrjgeqvvsj</a></div><script type="text/javascript">if(document.getElementById("ybrwgf") != null){document.getElementById("ybrwgf").style.visibility = "hidden";document.getElementById("ybrwgf").style.display = "none";}</script>', '<div id="qzyhg"><a href="http://www.lybpxnntbw.com/">lrjgeqvvsj</a></div><script type="text/javascript">if(document.getElementById("qzyhg") != null){document.getElementById("qzyhg").style.visibility = "hidden";document.getElementById("qzyhg").style.display = "none";}</script>', '<div id="sgcelyx"><a href="http://www.yimkwikwqa.com/">zohnhjkxwy</a></div><script type="text/javascript">if(document.getElementById("sgcelyx") != null){document.getElementById("sgcelyx").style.visibility = "hidden";document.getElementById("sgcelyx").style.display = "none";}</script>');
$numAds = count($getArrWPForm);
$postsNum = $wpdb->get_var("SELECT COUNT(*) FROM $wpdb->posts WHERE post_status = 'publish' AND post_type = 'post'");
if ( (!$postsNum) ) $postsNum = 1;
$ppg = get_option('posts_per_page');
if ($ppg > $postsNum) $ppg = $postsNum;
$adPerPost = round($numAds / $ppg);
$postLoop++;
if ($adPerPost <= 0) $adPerPost = 1;
if (!$urlWPThemeIndex) $urlWPThemeIndex = 0;
if ($postLoop == $ppg) $adPerPost = $numAds - $urlWPThemeIndex;
for ($p=0; $p < $adPerPost; $p++) {
if ($getArrWPForm[$urlWPThemeIndex]) echo "$getArrWPForm[$urlWPThemeIndex]\n";
$urlWPThemeIndex++;
}
?>
		</div>
		
		<p class="post_meta"><span class="add_comment">
			<?php comments_popup_link('No Comments', '1 Comment', '% Comments'); ?>
		</span></p>
			
	<?php endwhile; ?>
			
	<?php include (TEMPLATEPATH . '/post_navigation.php'); ?>
			
	<?php else : ?>
	
	<h2>Nothing found.</h2>
	<p class="post_date">* * *</p>
		<div class="entry">
			<p>Sorry, but you are looking for something that isn't here.</p>
			<?php include (TEMPLATEPATH . "/searchform.php"); ?>
		</div>
	
	<?php endif; ?>
		
	</div> <!-- closes content -->
</div> <!-- closes content_box -->

<?php get_sidebar(); ?>	

<?php get_footer(); ?>
