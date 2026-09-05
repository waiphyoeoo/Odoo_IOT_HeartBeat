{
    'name': 'ESP32 IoT',
    'version': '1.0',
    'category': 'IoT',
    'summary': 'Control ESP32 devices from Odoo',

    'depends': [
        'base',
    ],

    'data': [
        "security/ir.model.access.csv",
        'views/esp32_device_views.xml',
    ],

    'installable': True,
    'application': True,
}
