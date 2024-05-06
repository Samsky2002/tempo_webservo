#!/usr/bin/env python

print("Content-type: text/html\r\n")  # Required header for CGI scripts
print("\r\n")
html_content = """
<!DOCTYPE html>
<html>
<head>
    <title>Simple CGI Form</title>
</head>
<body>
    <h1>Enter Your Name</h1>
    <form method="post" action="cgi-bin/script.py">
        <label for="name">Name:</label>
        <input type="text" name="name" required>
        <input type="submit" value="Submit">
    </form>
    <hr>
    <h2>Greeting:</h2>
    <p>{greeting}</p>
</body>
</html>
"""

# Retrieve user input from the CGI environment
import cgi
form = cgi.FieldStorage()
user_name = form.getvalue("name", "")

# Display the HTML content with the user's name
print(html_content.format(greeting="Hello, {}!".format(user_name)))

