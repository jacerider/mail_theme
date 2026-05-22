CONTENTS OF THIS FILE
---------------------

 * Introduction
 * Requirements
 * Installation
 * How it works
 * Color variables
 * Sending as HTML


INTRODUCTION
------------

Provides email wrapping HTML and styling.


REQUIREMENTS
------------

This module requires the Mailsystem module.


INSTALLATION
------------

Install as you would normally install a contributed Drupal module. Visit
https://www.drupal.org/node/1897420 for further information.


HOW IT WORKS
------------

Every outgoing email is wrapped in an HTML template through hook_mail_alter().

If your theme has a logo-email.png file at its root, that image is used in the
email header.


COLOR VARIABLES
----------------

The message body may reference the wrapper's color variables with a
{{ variable }} placeholder, for example:

  <a style="color: {{ primary_bg }}">Link</a>

This works in any email body, including the account email templates at
/admin/config/people/accounts. Available placeholders:

  {{ body_bg }}       {{ content_bg }}      {{ content_color }}
  {{ header_bg }}     {{ header_color }}
  {{ primary_bg }}    {{ primary_color }}
  {{ secondary_bg }}  {{ secondary_color }}

Only these color variables are substituted, and the body is never evaluated as
a Twig template - it is a plain allow-list substitution, safe to expose in the
admin UI. Unknown placeholders are left untouched.


SENDING AS HTML
---------------

Mail Theme produces HTML, but the configured mail *formatter* runs after
hook_mail_alter() and can convert the body back to plain text. Two setups are
supported:

 1. Keep your existing formatter and make sure it preserves HTML. With the
    SMTP module, enable "Send emails as HTML" (the smtp_allowhtml setting) on
    its configuration page.

 2. Use the bundled "Mail Theme (HTML formatter)" plugin. On the Mail System
    (mailsystem) settings page, set it as the Formatter and leave your sender
    (PHP mailer, SMTP Mailer, ...) unchanged. This formatter never strips the
    HTML, so no per-sender option is required.
