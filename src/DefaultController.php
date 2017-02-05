<?php
namespace Drupal\autocreategroup;

/**
 * Default controller for the autocreategroup module.
 */
class DefaultController extends ControllerBase {

  public function autocreategroup_settings($settings = 'general') {
    $types = [];
    $group_types = [];

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
        $output = t('There are no group content types that can be automatically ' . 'created. Click <a href="@add_types">here</a> to add a ' . 'group content type. Click <a href="@types">here</a> to ' . 'select from an existing list of content ' . 'types and make it a group content type.', [
          '@add_types' => \Drupal\Core\Url::fromRoute('node.type_add'),
          '@types' => \Drupal\Core\Url::fromRoute('entity.node_type.collection'),
        ]);
      }

        //If there are no active content types, display a message
      elseif (count($types) == 0) {
        $output = t('There are no content types for which you can automatically ' . 'create groups. The content types that are neither group ' . 'type nor can be posted in a group are the ones ' . 'for which you can automatically create groups. Click ' . '<a href="@add_types">here</a> to add a content type. Click ' . '<a href="@types">here</a> to select from an existing list of content types.', [
          '@add_types' => \Drupal\Core\Url::fromRoute('node.type_add'),
          '@types' => \Drupal\Core\Url::fromRoute('entity.node_type.collection'),
        ]);
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
      $output = t('Currently, there are no content types. Click ' . '<a href="@add_types">here</a> to add some content types. ' . 'Make sure you have at least one group node before trying to ' . 'automatic creation of groups per content type.', [
        '@add_types' => \Drupal\Core\Url::fromRoute('node.type_add')
        ]);
    }

    return $output;
  }

}
