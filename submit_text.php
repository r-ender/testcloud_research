<?php
    if (isset($_POST['s1'])) {
      echo $_POST['name']."\n";
      echo $_POST['email']."\n";
      echo "<br><br>";
      echo nl2br("Thank you for your message, it will be processed soon.");
      
      $myfile = fopen("/home/cloud/Desktop/website/dorlan/submit-test/submitted.txt", "a") or die("Unable to open file");
      
      $timestamp = date('m-d-Y_H:i:s')."\n";
      fwrite($myfile, $timestamp);
      
      $name = $_POST['name']."\n";
      fwrite($myfile, $name);

      $email = $_POST['email']."\n\n";
      fwrite($myfile, $email);
      fclose($myfile);
?>

    
<?php
    header( "Refresh:3; url=index.html", true, 303);
    exit;
    
?>    
<?php
}
?>
