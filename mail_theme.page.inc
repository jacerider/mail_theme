<?php

/**
 * @file
 * Contains mail_theme.page.inc.
 *
 * Theme callback for mail theme.
 */

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
    // If we are using .png, change to .svg.
    $logo = str_replace('.png', '-email.png', theme_get_setting('logo.url', $config->get('default')));
    $logo = str_replace('.svg', '-email.png', $logo);
    if (file_exists(ltrim($logo, '/'))) {
      $url = Url::fromUserInput($logo);
      $variables['logo_url'] = $url->setAbsolute()->toString();
      [$width, $height] = getimagesize($url->toString());
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

  $variables['primary_color'] = '#226127';
  $variables['secondary_color'] = '#56bceb';
  if (function_exists('exo_theme_colors')) {
    $colors = exo_theme_colors();
    $variables['content_color'] = $colors['base']['hex'] ?? $variables['text_color'];
    $variables['primary_color'] = $colors['primary']['hex'] ?? $variables['primary_color'];
    $variables['secondary_color'] = $colors['secondary']['hex'] ?? $variables['secondary_color'];
  }
}
