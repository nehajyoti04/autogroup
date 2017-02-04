<?php

/**
 * @filesource
 *   Define admin settings for autocreategroup module.
 */

/**
 * Callback Functions for General and Advanced Settings of Auto Creation of Groups
 * General Settings to assocaite groups with active content types
 * Advanced Settings to customize every combination of active and group content types
 *
 * @param String $settings
 *   String containing type of settings to be loaded.
 */
function autocreategroup_settings($settings = 'general') {
  $types = array();
  $group_types = array();

  $types = _autocreategroup_classify_content_types('active');
  $group_types = _autocreategroup_classify_content_types('group');

  if (is_array($types) && is_array($group_types)) {
    // if (is_array($types)) {
      // If there are active and group content types create and render the general and advanced settings accordingly
      if (count($types) > 0 && count($group_types) > 0) {
        switch ($settings) {
          case 'general':
            // User is on general settings page.
            $output = drupal_get_form('autocreategroup_settings_general', $types);
            break;
          case 'advanced':
            // User is on advanced settings page.
            $output = drupal_get_form('autocreategroup_settings_advanced', $group_types);
            break;
        }
      }

      // If there are no group content types display a message
      elseif (count($group_types) == 0) {
        $output = t('There are no group content types that can be automatically ' .
                    'created. Click <a href="@add_types">here</a> to add a ' .
                    'group content type. Click <a href="@types">here</a> to ' .
                    'select from an existing list of content ' .
                    'types and make it a group content type.',
                    array(
                      '@add_types' => url('admin/structure/types/add'),
                      '@types' => url('admin/structure/types')
                    ));
      }

      //If there are no active content types, display a message
      elseif (count($types) == 0) {
        $output = t('There are no content types for which you can automatically ' .
                    'create groups. The content types that are neither group ' .
                    'type nor can be posted in a group are the ones ' .
                    'for which you can automatically create groups. Click ' .
                    '<a href="@add_types">here</a> to add a content type. Click ' .
                    '<a href="@types">here</a> to select from an existing list of content types.',
                    array(
                      '@add_types' => url('admin/structure/types/add'),
                      '@types' => url('admin/structure/types')
                    ));
      }
    /* }

    // If there is only one content type, display a message
    else {
      $output = t('There are not enough content types to create groups automatically. ' .
                    'To add more content types click <a href="@add_types">here</a>. ' .
                    'Make sure you have at least one group node before trying ' .
                    'to configure automatic creation of groups per content type.',
                    array(
                      '@add_types' => url('admin/structure/types/add'),
                    ));
    } */
  }

  // If there are no content types, display a message
  else {
    $output = t('Currently, there are no content types. Click ' .
                '<a href="@add_types">here</a> to add some content types. ' .
                'Make sure you have at least one group node before trying to ' .
                'automatic creation of groups per content type.',
                array(
                  '@add_types' => url('admin/structure/types/add'),
                ));
  }

  return $output;
}

/**
 * Function to return the form for general settings after validating the
 * presence of active and group content types
 * @param Array $form
 *   Form displayed to user/admin
 *
 * @param Array $form_state
 *   submitted values in form.
 *
 * @param Array $types
 *   An array containing all bundles with details, which are not enabled as Group content.
 *
 * @see autocreategroup_settings_general_submit()
 */
function autocreategroup_settings_general($form, &$form_state, $types) {
  $active_types = _autocreategroup_classify_content_types('active');
  $group_types = _autocreategroup_classify_content_types('group');

  // browse all the active content types for which groups can be automatically created
  foreach ($active_types AS $key => $active_type) {
    $active_name = $active_type['bundle'];

    // fieldset for every active content type
    $form[$active_type['bundle_name']] = array(
      '#type' => 'fieldset',
      '#title' => t('Automatically Create Groups for every @name',
                    array('@name' => $active_name)),

      '#weight' => 5,
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#description' => t('Automatically create the following groups for every ' .
                          '<em>@name</em>', array('@name' => $active_name)),
    );
    $options = array();
    $default_options = array();

    // browse all the content types which are treated as group nodes
    foreach ($group_types as $group_type) {
      $group_name = $group_type['bundle'];
      $options[$group_type['bundle_name']] = $group_name;
    }

    // checkboxes with group names as options for each active content type
    $form[$active_type['bundle_name']][$active_type['bundle_name'] . '_autocreategroups'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Available group types'),
      '#default_value' => variable_get($active_type['bundle_name'] . '_autocreategroups', array()),
      '#options' => $options,
    );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Settings'),
    '#weight' => 10,
  );

  return $form;
}

/**
 * Function to return the form for advanced settings after validating
 * the presence of active and group content types.
 *
 * @param Array $form
 *   Form displayed to user/admin
 *
 * @param Array $form_state
 *   submitted values in form.
 *
 * @param Array $enabled_types
 *   An array containing all bundles with details, which are enabled as Group content.
 *
 * @see _autocreategroup_validate_og_private()
 *      autocreategroup_settings_advanced_submit()
 */
function autocreategroup_settings_advanced($form, &$form_state, $enabled_types) {
  $active_types = array();
  $group_types = array();
  $has_data = FALSE;

  // Get active bundles
  $active_types = _autocreategroup_classify_content_types('active');

  // Get bundles selected as group content.
  $group_types = _autocreategroup_classify_content_types('group');

  // browse all the active content types for which groups can be automatically created
  foreach ($active_types as $active_type_value) {
    // Get bundle machine name
    $active_type = $active_type_value['bundle_name'];
    // Get bundle label
    $active_name = $active_type_value['bundle'];

    $options = array();
    $default_options = array();

    // browse all the active groups for the current content type
    $active_groups = variable_get($active_type . '_autocreategroups', '');

    if (is_array($active_groups) && count($active_groups) > 0) {
      foreach ($active_groups as $active_group) {

        if ($active_group !== 0) {
          // Set has data to TRUE if there is atleast on bundle which has selected.
          $has_data = TRUE;
          // Get bundle name
          $active_group_name = _autocreategroup_get_bundle_name($active_group);

          // Get group_group field details from DB.
          $group_group_field_details_query = db_select('field_config_instance', 'fci');
          $group_group_field_details_query->condition('fci.field_name', OG_GROUP_FIELD, '=')
                                          ->condition('fci.bundle', $active_group, '=')
                                          ->fields('fci', array('data'));
          $group_group_field_details_result = $group_group_field_details_query->execute()->fetchField();

          // Once we've all data related to field then unserialize.
          $group_group_field_details = unserialize($group_group_field_details_result);

          // fieldset for every active content type and group type
          $form[$active_type . '_' . $active_group] = array(
            '#type' => 'fieldset',
            '#title' => t('@group for @name',
                          array(
                            '@group' => $active_group_name,
                            '@name' => $active_name
                          )),

            '#weight' => 5,
            '#collapsible' => TRUE,
            '#collapsed' => FALSE,
            '#description' => t('Customize settings for automatically creating ' .
                                '@group for every @name',
                                array(
                                  '@group' => $active_group_name,
                                  '@name' => $active_name
                                )),
          );

          // Customize Title of the group
          $form[$active_type . '_' . $active_group][$active_type . '_' . $active_group . '_autocreategroup_og_title'] = array(
            '#type' => 'textfield',
            '#title' => t('Title'),
            '#default_value' => variable_get($active_type . '_' . $active_group . '_autocreategroup_og_title', '[node-title] ' . $active_group_name),
            '#size' => 60,
            '#maxlength' => 128,
            '#required' => TRUE,
            '#weight' => 6,
            '#description' => t('Use [node-title] as a token to be replaced by ' .
                                'the title of the node.'),
          );

          // Customize Description of the group
          $form[$active_type . '_' . $active_group][$active_type . '_' . $active_group . '_autocreategroup_og_description'] = array(
            '#type' => 'textfield',
            '#title' => t('Description'),
            '#default_value' => variable_get($active_type . '_' . $active_group . '_autocreategroup_og_description', 'This ' . $active_group_name . ' is dedicated to [node-title].'),
            '#size' => 70,
            '#maxlength' => 150,
            '#required' => TRUE,
            '#weight' => 7,
            '#description' => t('Use [node-title] as a token to be replaced by ' .
                                'the title of the node.'),
          );

          // Customize Group field value of group
          $form[$active_type . '_' . $active_group][$active_type . '_' . $active_group . '_autocreategroup_og_' . OG_GROUP_FIELD] = array(
            '#type' => 'checkbox',
            '#title' => t('@label', array('@label' => $group_group_field_details['label'])),
            '#default_value' => variable_get($active_type . '_' . $active_group . '_autocreategroup_og_' . OG_GROUP_FIELD, TRUE),
            '#weight' => 8,
            '#description' => t('Group checkbox field value in @bundle. <br /> @field_desc',
                                array(
                                  '@bundle' => $active_group_name,
                                  '@field_desc' => $group_group_field_details['description']
                                ))
          );

          // Should the group appear in public listing
          /* $form[$active_type . '_' . $active_group][$active_type . '_' . $active_group . '_autocreategroup_og_directory'] = array(
            '#type' => 'checkbox',
            '#title' => t('List in groups directory'),
            '#default_value' => variable_get($active_type . '_' . $active_group . '_autocreategroup_og_directory', 0),
            '#weight' => 8,
            '#description' => t('Should this group appear on the <a href="@og_link">list of groups page</a> ' .
                                '(requires OG Views module)? Disabled if the ' .
                                'group is set to <em>private group</em>.',
                                array(
                                  '@og_link', url('og')
                                )),
          );

          $options = array(
            t('Open - membership requests are accepted immediately.'),
            t('Moderated - membership requests must be approved.'),
            t('Invite only - membership must be created by an administrator.'),
            t('Closed - membership is exclusively managed by an administrator.'),
          );

          // Customize Membership Requests of the group
          $form[$active_type . '_' . $active_group][$active_type . '_' . $active_group . '_autocreategroup_og_selective'] = array(
            '#type' => 'radios',
            '#title' => t('Membership requests'),
            '#required' => TRUE,
            '#default_value' => variable_get($active_type . '_' . $active_group . '_autocreategroup_og_selective', 0),
            '#options' => $options,
            '#weight' => 9,
            '#description' => t('How should membership requests be handled in ' .
                                'this group? When you select <em>closed</em>, ' .
                                'users will not be able to join <strong>or</strong> leave.'),
          );

          // Should the group be provate
          $form[$active_type . '_' . $active_group][$active_type . '_' . $active_group . '_autocreategroup_og_private'] = array(
            '#type' => 'checkbox',
            '#title' => t('private Group'),
            '#default_value' => variable_get($active_type . '_' . $active_group . '_autocreategroup_og_private', 0),
            '#weight' => 10,
            '#description' => t('Should this group be visible only to its ' .
                                'members? Disabled if the group is set to ' .
                                '<em>List in Directory</em> or <em>Membership ' .
                                'requests: open</em>.'),

            '#element_validate' => array('_autocreategroup_validate_og_private'),
          );

          // Customize Membership Requests of the group
          $form[$active_type . '_' . $active_group][$active_type . '_' . $active_group . '_autocreategroup_og_register'] = array(
            '#type' => 'checkbox',
            '#title' => t('Registration form'),
            '#default_value' => variable_get($active_type . '_' . $active_group . '_autocreategroup_og_register', 0),
            '#weight' => 11,
            '#description' => t('May users join this group during registration? ' .
                                'If checked, a corresponding checkbox will be ' .
                                'added to the registration form.'),
          );*/
        }
      }
    }
  }

  // If there are no bundles which are selected then show user message to select one, else show submit button.
  if (empty($has_data)) {
    // fieldset for every active content type and group type
    $form['autocreategroup_advanced'] = array(
      '#type' => 'fieldset',
      '#title' => t('Advanced Settings'),
      '#weight' => 5,
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#description' => t('Please click <a href="@link">here</a> and proceed to select group ' .
                          'types that need to be automatically created for ' .
                          'the active content types.',
                          array(
                            '@link' => url('admin/config/autocreategroup/general')
                          )),
    );
  }
  else {
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save Settings'),
      '#weight' => 20,
    );
  }

  return $form;
}

/**
 * Function to validate private groups
 *
 * @param Array $element
 * @param Array $form_state
 *
 * @see autocreategroup_settings_advanced()
 *      autocreategroup_settings_advanced_submit()
 */
function _autocreategroup_validate_og_private($element, &$form_state) {
  $directory_listing = str_replace('private', 'directory', $element['#name']);
  $membership_type = str_replace('private', 'selective', $element['#name']);

  if ($element['#value'] == 1 && $form_state['values'][$directory_listing] == 1) {
    form_set_error($element['#name'],
      t('@element_name Listing to directory and private group cannot exist simultaneously.',
        array('@element_name' => $element['#name']))
    );
  }

  if ($element['#value'] == 1 && $form_state['values'][$membership_type] < 1) {
    form_set_error($element['#name'], t('Open Membership and private group cannot exist simultaneously.'));
  }
}

/**
 * Submit handler for general settings
 *
 * @param Array $form
 * @param Array $form_state
 *
 * @see autocreategroup_settings_general()
 */
function autocreategroup_settings_general_submit($form, &$form_state) {
  $output = '';

  // Loop through submissions
  foreach ($form_state['values'] as $key => $value) {
    if (strpos($key, '_autocreategroups') !== FALSE) {
      variable_set($key, $value);
      // Get all bundles selected as Group content.
      $group_types = _autocreategroup_classify_content_types('group');

      foreach ($group_types as $group) {
        $og_variables = str_replace('autocreategroups', $group['bundle_name'], $key);
        $exists = ($value[$group['bundle_name']] !== 0) ? 1 : 0;

        switch ($exists) {
          // If user selects any bundle in general settings
          // then set all required custom variables in variable table.
          case 1:
            variable_set($og_variables . '_autocreategroup_og_title', '[node-title] ' . $value[$group['bundle_name']]);
            variable_set($og_variables . '_autocreategroup_og_description', 'This ' . $value[$group['bundle_name']] . ' is dedicated to [node-title].');
            variable_get($og_variables . '_autocreategroup_og_' . OG_GROUP_FIELD, $value[$group['bundle_name']], TRUE);
            variable_set($og_variables . '_autocreategroup_og_directory', 0);
            variable_set($og_variables . '_autocreategroup_og_selective', '0');
            variable_set($og_variables . '_autocreategroup_og_private', 0);
            variable_set($og_variables . '_autocreategroup_og_register', 0);
            break;
          // If user unselect any bundle in general settings then delete all previously defined variables.
          case 0:
            variable_del($og_variables . '_autocreategroup_og_title');
            variable_del($og_variables . '_autocreategroup_og_description');
            variable_get($og_variables . '_autocreategroup_og_' . OG_GROUP_FIELD);
            variable_del($og_variables . '_autocreategroup_og_directory');
            variable_del($og_variables . '_autocreategroup_og_selective');
            variable_del($og_variables . '_autocreategroup_og_private');
            variable_del($og_variables . '_autocreategroup_og_register');
            break;
        }
      }
    }
  }

  drupal_set_message(t('Your settings have been saved. @output', array('@output' => $output)));
}

/**
 * Submit handler for advanced settings
 *
 * @param Array $form
 * @param Array $form_state
 *
 * @see autocreategroup_settings_advanced()
 *      _autocreategroup_validate_og_private()
 */
function autocreategroup_settings_advanced_submit($form, &$form_state) {
  // Loop through submitted values and set them in variable tables.
  foreach ($form_state['values'] as $key => $value) {
    // If the field is custom defined by our module then update in variable table.
    if (strpos($key, '_og_') !== FALSE) {
      variable_set($key, $value);
    }
  }

  drupal_set_message(check_plain('Your settings have been saved.'));
}