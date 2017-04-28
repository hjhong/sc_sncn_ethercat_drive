SOMANET EtherCAT Drive
======================

The **SOMANET EtherCAT Drive Library** offers the services and utilities required to perform BLDC/BDC Motor Control over EtherCAT using SOMANET devices.
It implements a CiA 402-compliant Motor Control protocol that gathers all the functionalities offered by the :ref:`SOMANET Motor Control Library<somanet_motor_control>` and gives flexibility for application-specific requirements.

.. cssclass:: downloadable-button 

  `Download Library <https://github.com/synapticon/sc_sncn_ethercat_drive/archive/release_2.0.0.zip>`_

.. cssclass:: github

  `Visit Public Repository <https://github.com/synapticon/sc_sncn_ethercat_drive/>`_

.. _ecat_drive_hw_compatibility:

Hardware Compatibility
----------------------

.. class:: float-left 

+---------------------------+
| Required SOMANET Hardware |
+===========================+
| 1x SOMANET **Core**       |
+---------------------------+
| 1x SOMANET **COM**        |
+---------------------------+
| 1x SOMANET **IFM**        |
+---------------------------+

.. class:: float-left 

+-----------------------------------------------------------------------------------------------+
| Supported SOMANET Devices                                                                     |
+===============================================================================================+
| SOMANET COM: :ref:`EtherCAT <com_ethercat>`                                                   |
+-----------------------------------------------------------------------------------------------+
| SOMANET Core: :ref:`C22 <core_c22>`                                                           |
+-----------------------------------------------------------------------------------------------+
| SOMANET IFM: :ref:`DC 100 <ifm_dc100>`, :ref:`DC 300 <ifm_dc300>`, **DC 1000**, **DC 5000**   |
+-----------------------------------------------------------------------------------------------+

Modules
-------

.. toctree::
    :maxdepth: 1
    :hidden:

    Drive Module <module_ethercat_drive/doc/index>

    * `Drive Module <module_ethercat_drive/doc/index.html>`_: Provides a Service that acts as a joint for EtherCAT and Motor Control Libraries and allows driving motors over EtherCAT.

EtherCAT Drive Linux Master Libraries
-------------------------------------

.. toctree::
    :maxdepth: 1
    :hidden:

    Linux Control Protocol Library <lib_linux_ctrlproto/doc/index>
    Linux Motor Drive Library <lib_linux_motor_drive/doc/index>

    * `Linux Control Protocol Library <lib_linux_ctrlproto/doc/index.html>`_: Provides functionality to your EtherCAT Master app to handle basic communication with your SOMANET EtherCAT slave.
    * `Linux Motor Drive Library <lib_linux_motor_drive/doc/index.html>`_: Provides functionality to your EtherCAT Master app to drive motors using your SOMANET EtherCAT slave.

Examples
--------

.. toctree::
    :hidden:
    :maxdepth: 1

    Drive Slave Firmware <examples/app_demo_slave_ethercat_motorcontrol/doc/index>
    
    Cyclic Position Control Demo <examples/app_demo_master_cyclic_position/doc/index>
    Cyclic Velocity Control Demo <examples/app_demo_master_cyclic_velocity/doc/index>
    Cyclic Torque Control Demo <examples/app_demo_master_cyclic_torque/doc/index>

    Dual-node Cyclic Position Control Demo <examples/app_demo_master_cyclic_position_2_nodes/doc/index>
    Dual-node Cyclic Velocity Control Demo <examples/app_demo_master_cyclic_velocity_2_nodes/doc/index>
    Dual-node Cyclic Torque Control Demo <examples/app_demo_master_cyclic_torque_2_nodes/doc/index>

    PDO Handling Demo <examples/app_demo_slave_pdo_handling/doc/index>
    PDO Handling Demo Master <examples/app_linux_ctrlproto-master-example/doc/index>

* **Slave examples (for SOMANET devices):**

    * `Drive Slave Firmware <examples/app_demo_slave_ethercat_motorcontrol/doc/index.html>`_: EtherCAT Drive slave firmware for your SOMANET device.


* **Master examples (for Linux machines):**

    * `Cyclic Position Control Demo <examples/app_demo_master_cyclic_position/doc/index.html>`_: Example to do cyclic position control on one axis over EtherCAT.
    * `Cyclic Velocity Control Demo <examples/app_demo_master_cyclic_velocity/doc/index.html>`_: Example to do cyclic velocity control on one axis over EtherCAT.
    * `Cyclic Torque Control Demo <examples/app_demo_master_cyclic_torque/doc/index.html>`_: Example to do cyclic torque control on one axis over EtherCAT.

    * `Dual-node Cyclic Position Control Demo <examples/app_demo_master_cyclic_position_2_nodes/doc/index.html>`_: Example to do cyclic position control on two axis over EtherCAT.
    * `Dual-node Cyclic Velocity Control Demo <examples/app_demo_master_cyclic_velocity_2_nodes/doc/index.html>`_: Example to do cyclic velocity control on two axis over EtherCAT.
    * `Dual-node Cyclic Torque Control Demo <examples/app_demo_master_cyclic_torque_2_nodes/doc/index.html>`_: Example to do cyclic torque control on two axis over EtherCAT.

    * `PDO Handling Demo Slave <examples/app_demo_slave_pdo_handling/doc/index.html>`_: Showcases simple PDO communication (Slave)
    * `PDO Handling Demo Master <examples/app_linux_ctrlproto-master-example/doc/index.html>`_: Showcases simple PDO communication (Master)

Dependencies
------------

To run **EtherCAT Drive applications** it is necessary to include additionally the following libraries:

* :ref:`SOMANET Motor Control <somanet_motor_control>`

    .. cssclass:: downloadable-button 

     `Download SOMANET Motor Control Library <https://github.com/synapticon/sc_sncn_motorcontrol/archive/master.zip>`_

    .. cssclass:: github

      `Visit SOMANET Motor Control Public Repository <https://github.com/synapticon/sc_sncn_motorcontrol>`_

* :ref:`SOMANET EtherCAT <somanet_ethercat>`

    .. cssclass:: downloadable-button 

     `Download SOMANET EtherCAT Library  <https://github.com/synapticon/sc_sncn_ethercat/archive/master.zip>`_

    .. cssclass:: github

      `Visit SOMANET EtherCAT Public Repository <https://github.com/synapticon/sc_sncn_ethercat>`_

* :ref:`SOMANET Base <somanet_base>`

    .. cssclass:: downloadable-button 

      `Download SOMANET Base Library <https://github.com/synapticon/sc_somanet-base/archive/master.zip>`_

    .. cssclass:: github

      `Visit SOMANET Base Public Repository <https://github.com/synapticon/sc_somanet-base>`_


.. important:: To be able to **run** EtherCAT Drive Master applications in your Linux machine, you first have to install all necessary **drivers**.
    
    Visit our :ref:`IgH EtherCAT Master for Linux Documentation <ethercat_master_software_linux>` for further information. 
