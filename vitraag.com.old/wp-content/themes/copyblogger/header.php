<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">

<head profile="http://gmpg.org/xfn/11">
	<meta http-equiv="Content-Type" content="<?php bloginfo('html_type'); ?>; charset=<?php bloginfo('charset'); ?>" />

	<title><?php if (is_home()) { bloginfo('description'); } else { wp_title('',true); } ?> &#8212; <?php bloginfo('name'); ?></title>

	<meta name="generator" content="WordPress <?php bloginfo('version'); ?>" /> <!-- leave this for stats -->
        <link rel="shortcut icon" href=""<?php bloginfo('url'); ?>/favicon.ico" >
        <link rel="icon" type="image/gif" href="<?php bloginfo('url'); ?>/favicon.gif" >

	<link rel="stylesheet" href="<?php bloginfo('stylesheet_url'); ?>" type="text/css" media="screen" />
	<link rel="stylesheet" href="<?php bloginfo('template_url'); ?>/custom.css" type="text/css" media="screen" />
	<!--[if lte IE 7]>
	<link rel="stylesheet" type="text/css" href="<?php bloginfo('template_url'); ?>/ie7.css" />
	<![endif]-->
	<!--[if lte IE 6]>
	<link rel="stylesheet" type="text/css" href="<?php bloginfo('template_url'); ?>/ie6.css" />
	<![endif]-->
	<link rel="alternate" type="application/rss+xml" title="<?php bloginfo('name'); ?> RSS Feed" href="<?php bloginfo('rss2_url'); ?>" />
	<link rel="pingback" href="<?php bloginfo('pingback_url'); ?>" />
	
	<script type="text/javascript" src="http://use.typekit.com/hwg5vqs.js"></script>
	<script type="text/javascript">try{Typekit.load();}catch(e){}</script>

	<?php wp_head(); ?>
</head>
<body class="custom">

<div id="header">
	<div id="logo">
	     <div class="inline">
		<img src="http://www.vitraag.com/images/letter.png" alt="Vitraag" class="inline"/>
		<a href="<?php bloginfo('url'); ?>" title="<?php bloginfo('name'); ?>"><span class="tk-le-havre"><?php bloginfo('name'); ?></span></a>
		<?php if (is_home()) { ?>
		<h1><?php bloginfo('description'); ?></h1>
		<?php } else { ?>	
		<p id="tagline" class="tk-le-havre"><?php bloginfo('description'); ?></p>
		<?php } ?> 
 	     </div>
	</div>
	
	<p id="rss"><a href="<?php bloginfo('rss2_url'); ?>" title="Subscribe to this site's feed"></a></p>
	
	<div id="nav"  class="tk-le-havre">
		<ul>
			<?php include (TEMPLATEPATH . '/nav_menu.php'); ?>
		</ul>			
	</div>
</div>
	
<div id="container">

