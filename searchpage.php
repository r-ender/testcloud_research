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
            <h1> Sieh dir untenstehend die Suchergebnisse an! /<br>
              Check out the search results below! :</h1>
            <div style="height:620px;width:45vw;overflow:auto; ">
              <div class="timeline">
                <div class="outer">

		<?php

		$search= $_POST['searchwords'];
		$search= trim($search);			//delete trailing whitespaces - protection against xss
		$searchoriginal= $search;
		$search= explode(" ", $search);		//separate input string into own elements
		$countsearchterms= count($search);

		echo "<h2>        Suchbegriffe / Searchwords: $searchoriginal </h2> <br><br>"; 

		//search is at the end of processing an array hence I must iterate over it
		foreach ($search as $key=>$value) {
			$processed_searchwd[$key] = $value;
		    //echo("foreach iteration: key = $key; value = $value ---> processed_searchwd[$key] = $processed_searchwd[$key] <br>");
		}

		if ($processed_searchwd[0] == NULL) $countsearchterms = 0;
		switch($countsearchterms)
		{
			case 1:
			$searchstring = "grep -ir $processed_searchwd[0] | grep -v \"index.html\" | grep -v \"<h\" | grep -v \".php\" | grep -v \".txt\"";
			break;

			case 2:
			$searchstring = "grep -ir $processed_searchwd[0] | grep -ir $processed_searchwd[1] | grep -v \"index.html\" | grep -v \"<h\" | grep -v \".php\" | grep -v \".txt\"";
			break;
			
			case 3:
			$searchstring = "grep -ir $processed_searchwd[0] | grep -ir $processed_searchwd[1] | | grep -ir $processed_searchwd[2] | grep -v \"index.html\" | grep -v \"<h\" | grep -v \".php\" | grep -v \".txt\"";
			break;
			
			case 4:
			$searchstring = "grep -ir $processed_searchwd[0] | grep -ir $processed_searchwd[1] | grep -ir $processed_searchwd[2] | grep -ir $processed_searchwd[3] | grep -v \"index.html\" | grep -v \"<h\" | grep -v \".php\" | grep -v \".txt\"";
			break;
			
			default:
			echo "Invalid search... redirecting to start page.";
			header( "Refresh:5; url=de/index.html", true, 303);
		    	exit;
		}


		$shelltest=shell_exec($searchstring);
		$expl_shellout = explode("\n", $shelltest);	//die Ausgabe in Zeilen unterteilen
		$line_count = count($expl_shellout)-1;

		if ($line_count <= 1)
		{
			$expl_shellout = implode($expl_shellout);
			$position = explode(":", $expl_shellout, 2);	
			$linkresult= $position[0];
			if( $position[1] == NULL){
				echo "Invalid search... redirecting to start page.";
				header( "Refresh:5; url=de/index.html", true, 303);
		    		exit;
			}				
			$linkcontent = substr($position[1],0,250).'...'; 	//limit output to 250 chars	
			echo ("<a href='$linkresult'>$linkcontent</a> <br><br>");
		}
		else
		{
			foreach ($expl_shellout as $value) {
				
				$position = explode(":", $value, 2);
				$linkresult= $position[0];
				if( $position[1] == NULL) continue;			//break current iteration and make next iteration
				$linkcontent = substr($position[1],0,250).'...'; 	//limit output to 250 chars
				
				//doppelte ergebnisse herausfiltern ( bei grep werden sie untereinander angezeigt, also muss man nur erstes mit nächstem vergleichen)
				if ( $tmp_linkresult != $linkresult && !strpos($linkcontent,"<img src=") ) 
					echo ("<div class=\"card\">
                    				<div class=\"info\">
                    				<a href='$linkresult'>$linkcontent</a>
                    				</div>
                  				</div>");
				
				$tmp_linkresult = $linkresult;
			}
		}

		      
		?>


                </div>
              </div>
            </div>
            
            <div class="search-form">
		<form action="searchpage.php" method="post">
		      <input type="text" name="searchwords" id="searchwords" placeholder="Suchbegriffe eingeben (max. 4)...">
		       <button type="submit"> <span style="font-size: 1.3vw; transform: translate(30%,60%);">&#128269;</span> </button>
	    	</form>
	    </div>
	    
            <!-- end of content column: put your content above this line --> </div>
        </div>
        <div id="tswothersidecol">
          <div id="tswothersideproper">
            <!-- start of left side column: insert your navigation menu, etc, here -->
            <div id="tswcssbuttons">
              <ul>
                <li><b><a href="de/index.html">Home</a></b></li>
                <li><b><a href="de/finanzen/index.html">Finanzen</a></b></li>
                <li><b><a href="de/immobilien/index.html">Immobilien</a></b></li>
                <li><b><a href="de/reisen/index.html">Reisen</a></b></li>
                <li><b><a href="de/technik/index.html"> <span style="padding-left: -25px;">Technik</span></a></b></li>
                <li><b><a href="de/leben/index.html">Leben</a></b></li>
                <li><b><a href="sitemap.html">About / Sitemap</a></b></li>
              </ul>
            </div>
            <div class="logo_img_container"> <img src="images/dorlan_logo2.png"
                alt="dorlan logo"> </div>
            <!-- end of side column: put your navigation menu, etc, above this line -->
          </div>
        </div>
        <div style="clear: both;"></div>
      </div>
      <div id="tswsidecol">
        <div id="tswsideproper">
          <!-- start of right side column: insert your navigation menu, etc, here -->
          <div id="youtube-link"> <a href="https://www.youtube.com"><img src="images/youtube_logo.png"
                style="mix-blend-mode: multiply;"></a> </div>
          <div id="insta-link"> <a href="https://www.instagram.com"><img src="images/insta_logo_schrift.png"
                style="mix-blend-mode: multiply;"></a> </div>
          <div id="d-a-ch"> <a href="de/index.html"><img src="images/d_a_ch.png"></a>
          </div>
          <div id="usa-gb"> <a href="en/index.html"><img src="images/usa_gb.png"></a>
          </div>
          <div id="donate">  <a href="https://www.savethechildren.org/us/where-we-work/africa">
              <p style="transform: translate(0,130%);"><span style="color: #ff0000;">
                  DONATE <br>
                  for <br>
                  Africa </span></p>
              <img src="images/africa_map.jpg" style="border-radius: 50%;"></a>
          </div>
          <!-- end of side column: put your navigation menu, etc, above this line -->
        </div>
      </div>
      <div style="clear: both;"></div>
      <div id="tswfooter">
        <!-- start of footer section: insert your copyright notice, etc, here -->
        <div id="footer_col_left">
          <div class="bottomtext"> <b> Schicke mir gerne eine Nachricht, Kollege,<br>
		egal ob Lob, Kritik, oder was du <br> mir sonst mitteilen möchtest :-)
            </div>
        </div>
        <div id="footer_col_right">
          <div class="contact-form" style="transform: translate(10%,0%);">
            
            <form method="post" action="contactform.php"> 
            	<label for="name">Name / E-Mail-Adr.</label>
              
              <input type="text" id="name" name="name" style="font-size: 1.5vw;" placeholder="Bitte nenne mir deinen Namen ..."> 
              <label for="subject">Betreff</label> 
              	<textarea name="subject" id="subject" type="text" style="font-size: 1.5vw;" placeholder="Was möchtest du mir mitteilen? ... " style="height:13vh;"></textarea>
              
              <input value="Senden" type="submit" name='s1' id='s1' class="btn btn-primary btn2" > </form>
          </div>
        </div>
        <!-- end of footer section: insert your copyright notice, etc, above this line -->
      </div>
    </div>
  </body>
</html>

