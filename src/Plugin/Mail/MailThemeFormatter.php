<?php

namespace Drupal\mail_theme\Plugin\Mail;

use Drupal\Core\Mail\MailInterface;

/**
 * Formats email with the Mail Theme HTML wrapper.
 *
 * Configure this plugin as the *formatter* on the Mail System (mailsystem)
 * settings page. Unlike the default "PHP mailer" or "SMTP Mailer" formatters,
 * it never converts the body to plain text, so the HTML produced by Mail Theme
 * is delivered intact. Combine it with any sender plugin (PHP mailer, SMTP
 * Mailer, ...).
 *
 * Using this formatter is optional. When it is not configured, Mail Theme
 * still wraps messages through hook_mail_alter() - but in that case the
 * configured formatter must be left HTML-safe (for example, by enabling
 * "Send emails as HTML" in the SMTP module).
 *
 * @Mail(
 *   id = "mail_theme",
 *   label = @Translation("Mail Theme (HTML formatter)"),
 *   description = @Translation("Wraps the message in the Mail Theme template and keeps it as HTML.")
 * )
 *
 * @see mail_theme_mail_alter()
 * @see _mail_theme_wrap_message()
 */
class MailThemeFormatter implements MailInterface {

  /**
   * {@inheritdoc}
   */
  public function format(array $message) {
    // Apply the Mail Theme wrapper. This is idempotent: if hook_mail_alter()
    // already wrapped the body, the call below is a no-op.
    _mail_theme_wrap_message($message);

    // Join the body into a single string. Crucially, the HTML is preserved -
    // it is never run through MailFormatHelper::htmlToText().
    if (is_array($message['body'])) {
      $message['body'] = implode("\n\n", $message['body']);
    }

    return $message;
  }

  /**
   * {@inheritdoc}
   */
  public function mail(array $message) {
    // This plugin only formats messages. A real sender plugin - such as "PHP
    // mailer" or "SMTP Mailer" - must be configured as the sender on the Mail
    // System (mailsystem) settings page.
    throw new \LogicException('The "mail_theme" mail plugin is a formatter only and cannot send mail. Select a separate sender plugin on the Mail System (mailsystem) settings page.');
  }

}
