<?php

/**
 * Implements hook_field_info().
 */
function panopoly_widgets_field_info() {
  return array(
    'panopoly_spotlight' => array(
      'label' => t('Spotlight'),
      'description' => t('A wonderfully widgetsal spotlight field'),
      'default_widget' => 'panopoly_spotlight_form',
      'default_formatter' => 'panopoly_spotlight_default',
      'settings' => array(
        'uri_scheme' => variable_get('file_default_scheme', 'public'),
        'default_image' => 1,
      ),
      'instance_settings' => array(
        'file_extensions' => 'jpg jpeg gif png',
        'file_directory' => 'general',
        'max_filesize' => '',
        'alt_field' => 0,
        'title_field' => 0,
        'max_resolution' => '',
        'min_resolution' => '',
      ),
    ),
  );
}

/**
 * Implements hook_field_is_empty()
 */
function panopoly_widgets_field_is_empty($item, $field) {
  if (empty($item['title']) && empty($item['link']) && empty($item['description']) && empty($item['fid'])) {
    return TRUE;
  }
  return FALSE;
}

/**
 * Implements hook_field_widget_info().
 */
function panopoly_widgets_field_widget_info() {
  return array(
    'panopoly_spotlight_form' => array(
      'label' => t('Spotlight'),
      'field types' => array('panopoly_spotlight'),
      'behaviors' => array(
        'multiple values' => FIELD_BEHAVIOR_DEFAULT,
        'default value' => FIELD_BEHAVIOR_DEFAULT,
      ),
    ),
  );
}

/**
 * Implements hook_field_settings_form()
 */
function panopoly_widgets_field_settings_form($field, $instance, $has_data) {
  $form = array();
  $defaults = field_info_field_settings($field['type']);
  $settings = array_merge($defaults, $field['settings']);

  $scheme_options = array();
  foreach (file_get_stream_wrappers(STREAM_WRAPPERS_WRITE_VISIBLE) as $scheme => $stream_wrapper) {
    $scheme_options[$scheme] = $stream_wrapper['name'];
  }
  $form['uri_scheme'] = array(
    '#type' => 'radios',
    '#title' => t('Upload destination'),
    '#options' => $scheme_options,
    '#default_value' => $settings['uri_scheme'],
    '#description' => t('Select where the final files should be stored. Private file storage has significantly more overhead than public files, but allows restricted access to files within this field.'),
  );

  // When the user sets the scheme on the UI, even for the first time, it's
  // updating a field because fields are created on the "Manage fields"
  // page. So image_field_update_field() can handle this change.
  $form['default_image'] = array(
    '#title' => t('Default image'),
    '#type' => 'managed_file',
    '#description' => t('If no image is uploaded, this image will be shown on display.'),
    '#default_value' => $field['settings']['default_image'],
    '#upload_location' => $settings['uri_scheme'] . '://default_images/',
  );

  return $form;
}

/**
 * Implements hook_field_instance_settings_form()
 */
function panopoly_widgets_field_instance_settings_form($field, $instance) {
  $form = image_field_instance_settings_form($field, $instance);
  return $form;
}

/**
 * Implementation of hook_field_formatter_info().
 */
function panopoly_widgets_field_formatter_info() {
  return array(
    'panopoly_spotlight_default' => array(
      'label' => t('Default'),
      'field types' => array('panopoly_spotlight'),
    ),
  );
}

/**
 * Implements hook_field_formatter_view().
 */
function panopoly_widgets_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $element = array();
  $settings = $display['settings'];
  $formatter = $display['type'];
  drupal_add_library('system', 'ui.tabs');

  // Assemble the tabs header
  $header = '<ul>';
  foreach($items as $delta => $item_data) {
    $header .= '<li>' . '<a href="#panopoly-spotlight-' . $delta . '">' . ($delta+1) . '</a></li>'; 
  }
  $header .= '</ul>';

  // Assemble the tabs content
  foreach($items as $delta => $item_data) {
    $element[$delta]['#markup'] = ($delta == 0) ? $header . theme('panopoly_spotlight_view', array('items' => $item_data, 'delta' => $delta)) : theme('panopoly_spotlight_view', array('items' => $item_data, 'delta' => $delta));
  }
  return $element;
}

/**
 * Implements hook_widget_form().
 */
function panopoly_widgets_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {

  if ($instance['widget']['type'] == 'panopoly_spotlight_form') {
   
    // Make it a multipart form  
    $form['#attributes']['enctype'] = 'multipart/form-data';

    // Define the element
    $element['title'] = array(
      '#title' => 'Title',
      '#type' => 'textfield',
      '#default_value' => isset($items[$delta]['title']) ? $items[$delta]['title'] : NULL,
    );

    $element['link'] = array(
      '#title' => 'Link',
      '#type' => 'textfield',
      '#default_value' => isset($items[$delta]['link']) ? $items[$delta]['link'] : NULL,
    );

    $element['fid'] = array(
      '#title' => 'Image',
      '#type' => 'managed_file',
      '#upload_location' => file_field_widget_uri($field, $instance),
      '#default_value' => isset($items[$delta]['fid']) ? $items[$delta]['fid'] : NULL,
    );

    $element['description'] = array(
      '#title' => 'Description',
      '#type' => 'textarea',
      '#rows' => '2',
      '#resizable' => FALSE,
      '#default_value' => isset($items[$delta]['description']) ? $items[$delta]['description'] : NULL,
    );
  }

  return $element;
}

/**
 * Implements hook_field_presave()
 */
function panopoly_widgets_field_presave($entity_type, $entity, $field, $instance, $langcode, &$items) {
  image_field_presave($entity_type, $entity, $field, $instance, $langcode, $items);
}

/**
 * Implements hook_field_load().
 */
function panopoly_widgets_field_load($entity_type, $entities, $field, $instances, $langcode, &$items, $age) {
  image_field_load($entity_type, $entities, $field, $instances, $langcode, $items, $age);
}

/**
 * Implements hook_field_insert().
 */
function panopoly_widgets_field_insert($entity_type, $entity, $field, $instance, $langcode, &$items) {
  image_field_insert($entity_type, $entity, $field, $instance, $langcode, $items);
}

/**
 * Implements hook_field_update().
 */
function panopoly_widgets_field_update($entity_type, $entity, $field, $instance, $langcode, &$items) {
  image_field_update($entity_type, $entity, $field, $instance, $langcode, $items);
}

/**
 * Implements hook_field_delete().
 */
function panopoly_widgets_field_delete($entity_type, $entity, $field, $instance, $langcode, &$items) {
  image_field_delete($entity_type, $entity, $field, $instance, $langcode, $items);
}

/**
 * Implements hook_field_delete_revision().
 */
function panopoly_widgets_field_delete_revision($entity_type, $entity, $field, $instance, $langcode, &$items) {
  image_field_delete_revision($entity_type, $entity, $field, $instance, $langcode, $items);
}

/**
 * Implements hook_field_prepare_view().
 */
function panopoly_widgets_field_prepare_view($entity_type, $entities, $field, $instances, $langcode, &$items) {
  // If there are no files specified at all, use the default.
  // TODO - make sure this works 
  foreach ($entities as $id => $entity) {
    if (empty($items[$id]) && $field['settings']['default_image']) {
      if ($file = file_load($field['settings']['default_image'])) {
        $items[$id][0] = (array) $file + array(
          'is_default' => TRUE,
          'alt' => '',
          'title' => '',
        );
      }
    }
  }
}

/**
 * Theme function for table view
 */
function theme_panopoly_spotlight_view($variables) {
  $title = $variables['items']['title'];
  $description = $variables['items']['description'];
  $image_entity = entity_uuid_load('file', array($variables['items']['uuid']));
  $image = file_load(array_pop($image_entity)->fid);
  $link = $variables['items']['link'];

  $output = '<div id="' . 'panopoly-spotlight-' . $variables['delta'] . '" class="' . 'panopoly-spotlight' . '">';
  $output .= theme('image_style', array('style_name' => 'panopoly_image_spotlight', 'path' => $image->uri));
  $output .= '<div class="panopoly-spotlight-wrapper">';
  $output .= '<h3 class="panopoly-spotlight-label">' . t('Spotlight') . '</h3>';
  $output .= '<div class="panopoly-spotlight-info">';
  $output .= '<h2>' . l($title, $link) . '</h2>';
  $output .= '<p>' . $description . '</p>';
  $output .= '</div>';
  $output .= '</div>';
  $output .= '</div>';
  return $output;
}
