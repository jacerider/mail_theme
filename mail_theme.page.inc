<?php

/**
 * @file
 * Contains mail_theme.page.inc.
 *
 * Theme callback for mail theme.
 */

use Drupal\Component\Render\MarkupInterface;
use Drupal\Component\Utility\DeprecationHelper;
use Drupal\Core\Render\Markup;
use Drupal\Core\Url;

/**
 * Prepares variables for membership email wrapper templates.
 *
 * Default template: membership-email-wrapper.html.twig.
 *
 * @param array $variables
 *   An associative array containing:
 *   - elements: An associative array containing the user information and any
 *   - attributes: HTML attributes for the containing element.
 */
function template_preprocess_mail_theme_wrapper(array &$variables) {
  $site_config = \Drupal::config('system.site');
  $config = \Drupal::config('system.theme');
  $variables['logo_url'] = NULL;
  $variables['site_name'] = $site_config->get('name');

  if (!empty($variables['show_logo'])) {
    // Read the active theme's logo path. theme_get_setting() was deprecated in
    // Drupal 11.3 in favour of the ThemeSettingsProvider service, which does
    // not exist on Drupal 10.3 - 11.2, so dispatch by core version.
    $default_theme = $config->get('default');
    $logo_setting = DeprecationHelper::backwardsCompatibleCall(
      currentVersion: \Drupal::VERSION,
      deprecatedVersion: '11.3.0',
      currentCallable: fn() => \Drupal::service('Drupal\Core\Extension\ThemeSettingsProvider')->getSetting('logo.url', $default_theme),
      deprecatedCallable: fn() => theme_get_setting('logo.url', $default_theme),
    );
    // Swap the standard logo for its email-specific variant (logo-email.png).
    $logo = str_replace('.png', '-email.png', $logo_setting);
    $logo = str_replace('.svg', '-email.png', $logo);
    $logo_path = ltrim($logo, '/');
    if (file_exists($logo_path)) {
      // Cache-bust the logo with its file modification time so an updated
      // logo is not served from a stale email-client or proxy cache.
      $url = Url::fromUserInput($logo, ['query' => ['v' => filemtime($logo_path)]]);
      [$width, $height] = getimagesize($logo_path);
      $variables['logo_url'] = $url->setAbsolute()->toString();
      $variables['logo_width'] = $width;
      $variables['logo_height'] = $height;
    }
  }

  $theme = \Drupal::service('theme.manager')->getActiveTheme();
  $path_resolver = \Drupal::service('extension.path.resolver');
  $path = $path_resolver->getPath('theme', $theme->getName());
  $css_path = $path . '/assets/css/email.css';
  if (file_exists($css_path)) {
    $variables['css'] = file_get_contents($css_path);
  }

  /** @var \Drupal\Core\Extension\ModuleHandlerInterface $moduleHandler */
  $moduleHandler = \Drupal::service('module_handler');
  // Neo support.
  if ($moduleHandler->moduleExists('neo_color')) {
    if ($scheme = neo_color_get_pallet('base')) {
      $variables['body_bg'] = $scheme->getShade(200)->getHex();
      $variables['header_color'] = $scheme->getShade(0)->getContentHex();
      $variables['header_bg'] = $scheme->getShade(0)->getHex();
      $variables['content_color'] = $scheme->getShade(0)->getContentHex();
      $variables['content_bg'] = $scheme->getShade(0)->getHex();
    }
    if ($scheme = neo_color_get_pallet('primary')) {
      $variables['primary_color'] = $scheme->getShade(500)->getContentHex();
      $variables['primary_bg'] = $scheme->getShade(500)->getHex();
    }
    if ($scheme = neo_color_get_pallet('secondary')) {
      $variables['secondary_color'] = $scheme->getShade(500)->getContentHex();
      $variables['secondary_bg'] = $scheme->getShade(500)->getHex();
    }
  }
  // Exo support.
  elseif ($moduleHandler->moduleExists('exo')) {
    $colors = exo_theme_colors();
    $variables['content_color'] = $colors['base']['hex'] ?? $variables['text_color'];
    $variables['primary_bg'] = $colors['primary']['hex'] ?? $variables['primary_bg'];
    $variables['secondary_bg'] = $colors['secondary']['hex'] ?? $variables['secondary_bg'];
  }

  // Allow the message body to reference the wrapper's color variables with a
  // {{ variable }} placeholder, e.g. {{ primary_bg }}.
  _mail_theme_replace_template_vars($variables);
}

/**
 * Replaces {{ variable }} placeholders in the email body with color values.
 *
 * This lets the message body - including the email templates entered at
 * /admin/config/people/accounts - reference the wrapper's color variables,
 * for example {{ primary_bg }}.
 *
 * For security, only an explicit allow-list of color variables is exposed and
 * the body is treated as a plain string: it is NEVER compiled or evaluated as
 * a Twig template, so admin-entered text cannot execute code. Unknown
 * placeholders are left untouched.
 *
 * @param array $variables
 *   The mail_theme_wrapper template variables, passed by reference.
 */
function _mail_theme_replace_template_vars(array &$variables) {
  if (empty($variables['body'])) {
    return;
  }

  // Resolve the body to a string. It may be a string/Markup object, or a
  // #markup render array.
  if (is_string($variables['body']) || $variables['body'] instanceof MarkupInterface) {
    $body = (string) $variables['body'];
    $is_render_array = FALSE;
  }
  elseif (is_array($variables['body']) && isset($variables['body']['#markup'])) {
    $body = (string) $variables['body']['#markup'];
    $is_render_array = TRUE;
  }
  else {
    return;
  }

  // Build the allow-list of placeholders from the wrapper's color variables.
  $tokens = [];
  foreach ([
    'body_bg', 'content_bg', 'content_color',
    'header_bg', 'header_color',
    'primary_bg', 'primary_color',
    'secondary_bg', 'secondary_color',
  ] as $name) {
    if (isset($variables[$name]) && is_scalar($variables[$name])) {
      $tokens[$name] = (string) $variables[$name];
    }
  }

  // Substitute each {{ variable }} placeholder. Unknown names are left as-is.
  $body = preg_replace_callback('/\{\{\s*(\w+)\s*\}\}/', static function (array $match) use ($tokens) {
    return $tokens[$match[1]] ?? $match[0];
  }, $body);

  if ($is_render_array) {
    $variables['body']['#markup'] = Markup::create($body);
  }
  else {
    $variables['body'] = Markup::create($body);
  }
}
