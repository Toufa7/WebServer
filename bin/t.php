<?php
session_start(); // start session to store the counter value
if(isset($_POST['increment'])) { // check if the increment button is clicked
    if(isset($_SESSION['counter'])) { // check if the counter variable is set in the session
        $_SESSION['counter']++; // increment the counter
    } else {
        $_SESSION['counter'] = 1; // set the counter to 1 if it's not set in the session
        echo "Counter set to 1"; // debug statement
    }
}
?>

<!DOCTYPE html>
<html>
<head>
    <title>Increment Counter Example</title>
</head>
<body>
    <h1>Counter: <?php echo isset($_SESSION['counter']) ? $_SESSION['counter'] : 0; ?></h1>
    <form method="POST">
        <button type="submit" name="increment">Increment Counter</button>
    </form>
</body>
</html>