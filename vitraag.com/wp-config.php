<?php
/** WordPress's config file **/
/** http://wordpress.org/   **/

// ** MySQL settings ** //
define('WP_CACHE', true); //Added by WP-Cache Manager
define('DB_NAME', 'vitraagblog');     // The name of the database
define('DB_USER', 'vaibhavb');     // Your MySQL username
define('DB_PASSWORD', 'vaibhav'); // ...and password
define('DB_HOST', 'db.vitraag.com');     // ...and the server MySQL is running on

// Change the prefix if you want to have multiple blogs in a single database.

$table_prefix  = 'wp_5cq40l_';   // example: 'wp_' or 'b2' or 'mylogin_'

// Change this to localize WordPress.  A corresponding MO file for the
// chosen language must be installed to wp-includes/languages.
// For example, install de.mo to wp-includes/languages and set WPLANG to 'de'
// to enable German language support.
define ('WPLANG', '');

/* Stop editing */

$server = DB_HOST;
$loginsql = DB_USER;
$passsql = DB_PASSWORD;
$base = DB_NAME;

define('ABSPATH', dirname(__FILE__).'/');

// Get everything else
require_once(ABSPATH . 'wp-includes/init.php');
require_once(ABSPATH.'wp-settings.php');
?>
