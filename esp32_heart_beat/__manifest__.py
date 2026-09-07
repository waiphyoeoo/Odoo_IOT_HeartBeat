{
    'name': 'ESP32 IoT',
    'version': '18.0.1.0.0',
    'category': 'IoT',
    'summary': 'Control ESP32 devices from Odoo',

    'author': 'BlackJack',
    'license': 'LGPL-3',

    'images': [
        'static/description/banner.png',
    ],

    'depends': [
        'base',
    ],

    'data': [
        'security/ir.model.access.csv',
        'views/esp32_device_views.xml',
    ],

    'installable': True,
    'application': True,
}