<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml">

<head>

<meta http-equiv="Content-Type" content="<?php bloginfo('html_type'); ?>; charset=<?php bloginfo('charset'); ?>" />

<title><?php if (is_home()) { bloginfo('description'); } 
		else { wp_title('',true); } ?> &#8212; 
       <?php bloginfo('name'); ?>
</title>
<meta name="generator" content="WordPress <?php bloginfo('version'); ?>" /> 
<link rel="shortcut icon" href="<?php bloginfo('url'); ?>/favicon.ico" />
<link rel="icon" type="image/gif" href="<?php bloginfo('url'); ?>/favicon.gif" />
<link rel="stylesheet" href="<?php bloginfo('stylesheet_url'); ?>" 
	type="text/css" media="screen" />
<link rel="stylesheet" href="<?php bloginfo('template_url'); ?>/style.css" 
	type="text/css" media="screen" />

<!--[if lte IE 7]>
<link rel="stylesheet" type="text/css" href="<?php bloginfo('template_url'); ?>/css/ie7.css" />
<![endif]-->

<!--[if lte IE 6]>
<link rel="stylesheet" type="text/css" href="<?php bloginfo('template_url'); ?>/css/ie6.css" />
<![endif]-->

<link rel="alternate" type="application/rss+xml" 
	title="<?php bloginfo('name'); ?> RSS Feed" href="<?php bloginfo('rss2_url'); ?>" />
<link rel="pingback" href="<?php bloginfo('pingback_url'); ?>" />

<link href='http://fonts.googleapis.com/css?family=Spinnaker' rel='stylesheet' type='text/css'>

<noscript>
    <link rel="stylesheet" href="<?php bloginfo('template_url'); ?>/css/mobile.min.css" />
</noscript>
<script type="text/javascript">
    // Edit to suit your needs.
    var ADAPT_CONFIG = {
        // Where is your CSS?
        path: '<?php bloginfo('template_url'); ?>/css/',

        // false = Only run once, when page first loads.
        // true = Change on window resize and page tilt.
        dynamic: true,

        // First range entry is the minimum.
        // Last range entry is the maximum.
        // Separate ranges by "to" keyword.
        range: [
    '0px    to 760px  = mobile.min.css',
    '760px  to 980px  = 720.min.css',
    '980px  to 1280px = 960.min.css',
    '1280px to 1600px = 1200.min.css',
    '1600px to 1940px = 1560.min.css',
    '1940px to 2540px = 1920.min.css',
    '2540px           = 2520.min.css'
  ]
  };
</script>

<script src="<?php bloginfo('template_url'); ?>/js/adapt.min.js" type="text/javascript"></script>

<script src="<?php bloginfo('template_url'); ?>/js/site.js" type="text/javascript"></script>

<script type="text/javascript" 
        src="http://www.google.com/jsapi"></script>
<script type="text/javascript">
  // You may specify partial version numbers, such as "1" or "1.3",
  //  with the same result. Doing so will automatically load the 
  //  latest version matching that partial revision pattern 
  //  (e.g. 1.3 would load 1.3.2 today and 1 would load 1.7.0).
  google.load("jquery", "1.7.0");
 
  google.setOnLoadCallback(function() {
  	defineNav();
  });
</script>

<!-- WP Generated header -->
<?php wp_head(); ?>
<!-- End WP Generated header -->

</head>

<body>

<!-- Main Header Container -->
<div class="header-container">
    <div class="container_12">
        <div id="header" class="grid_5">
		<h1><a href="#" title="<?php bloginfo('name'); ?>">
			<span><?php bloginfo('name'); ?></span>
		</a></h1>
		<p id="tagline"><?php bloginfo('description'); ?></p>
	</div>

	<div id="nav" class="grid_7 omega align-right">
		<ul>
			<?php include (TEMPLATEPATH . '/nav_menu.php'); ?>
		</ul>
	</div>
	<div id="rss" class="grid-12 omega align-right">
		<a href="<?php bloginfo('rss2_url'); ?>" 
			title="Subscribe to this site's feed"></a>
	</div>
    </div> <!-- container-12 -->
</div> <!-- header-container -->

<div class="container_12">
	<div class="article grid_9">

