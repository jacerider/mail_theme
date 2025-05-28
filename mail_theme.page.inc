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
}
