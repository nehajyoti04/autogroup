Auto create Group
------------------------

Description
-----------

This module allows auto creation of groups associated with a particular node.

The module allows the administrator to decide what group content types
should be automatically created for active content types.
Once a combination is selected, the module also allows the administrator
to set default organic group settings like title, description, etc.

Installation
------------

The "Auto create Group" module allows auto creation of groups,
hence depends on the OG (Organic Groups) module.
Ensure that OG modules are installed and enabled on the site.
To install, place the entire autocreategroup folder into your site's modules directory.
From the Drupal administration section, go to Administer -> Modules 
and enable the Auto create Group module and its dependencies.

Updates
-------

This version currently only update the Drupal 6 code to Drupal 7.
To update follow steps below:
1. Delete old directory
2. Place new module directory.
3. Run update.php file.

Configurations
--------------

To configure this module you will first need to configure the OG module and
Define atleast one bundle as Group content.
Once you've configured OG properly,
1. Go to Administer -> Configuration -> AutoCreateGroup.
2. Select the group bundles, listed in any normal bundle on creation of which
   you want the selected group bundle to be created.
3. Click on Save Settings.
4. Once you have selected group bundles to be created, navigate to Advanced Settings tab.
   Here you can select advanced settings, for the each group bundles,
   which will allow you to create group with specified details.
5. Click on Save Settings.

Once you have configured all bundles and grouped bundles in AutoCreateGroup,
it is time to try it out.
Go to Administer -> Content -> Add content -> Select content type which you've
selected in general settings tab.
Fill the required details and click on Save.


Known incompatibilities
-----------------------

N/A.

Issues
------

If you have any concerns or found any issue with this module please don't hesitate to add them in issue queue.

Maintainers
-----------
The Auto create Group was originally developed by:
Amit Vyas

Current maintainers:
Amit Vyas
Yogesh S. Chaugule
