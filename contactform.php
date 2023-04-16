<!DOCTYPE html>
<html lang="en">
  <head>
    <meta http-equiv="content-type" content="text/html; charset=UTF-8">
    <title>Dorlan teilt seine Interessen (Immobilien, Finanzen, Reisen , Leben, uvm. ...)</title>
    <meta name="Dorlan" content="Immobilien, Finanzen, Reisen , Leben, uvm.">
    <meta name="description" content="Immobilien, Finanzen, Reisen , Leben, Ziele uvm.">
    <meta name="keywords" content="Immobilien, Finanzen, Reisen , Leben, Ziele, Freunde, IT, Security, Dating, Liebe.">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="stylesheet" type="text/css" href="de/tswnavbar.css">
    <link rel="stylesheet" type="text/css" media="all" href="de/styles.css">
    <link rel="icon" type="image/jpg" href="images/dorlan_favicon.png">
  </head>
  <body style="       background-color: #ffff66 ;">
    <div id="tswcontainer">
      <div id="tswheader">
        <!-- start of header section: insert your logo, etc, here -->
        <div class="bridge_img_container"> <img src="images/bridges3.png" alt="together stronger">
          <div class="bridge_img_centeredtext"><b>D | O | R | L | A
              | N </b></div>
        </div>
        <!-- end of header section: insert your logo, etc, above this line --> </div>
      <div id="tswcentrecombo">
        <div id="tswcontent">
          <div id="tswcontproper">
            <!-- start of content column: insert your content here -->
            <h1>
            <?php
		    if (isset($_POST['s1'])) {
		      echo $_POST['name']."\n";
		      echo $_POST['subject']."\n";
		      echo "<br><br>";

		      echo "Thank you for your message, it will be processed soon.";
		      
		      $myfile = fopen("/var/www/dorlan/submitted.txt", "a") or die("Unable to open file");
		      
		      $timestamp = date('m-d-Y_H:i:s')."\n";
		      fwrite($myfile, $timestamp);
		      
		      $name = $_POST['name']."\n";
		      fwrite($myfile, $name);

		      $email = $_POST['subject']."\n\n";
		      fwrite($myfile, $email);
		      fclose($myfile);
		?>

		    
		<?php
		    header( "Refresh:3; url=de/index.html", true, 303);
		    exit;
		    
		?>    
		<?php
		}
		?>
		</h1>

          
            <!-- end of content column: put your content above this line --> </div>
        </div>
      </div>

    </div>
  </body>
</html>

