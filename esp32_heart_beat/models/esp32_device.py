from odoo import models, fields


class Esp32Device(models.Model):
    _name = 'esp32.device'
    _description = 'ESP32 IoT Device'

    name = fields.Char(
        string='Device Name',
        required=True,
        default='ESP32-S3'
    )

    ip_address = fields.Char(
        string='IP Address',
        required=True,
        default='192.168.1.11'
    )

    status = fields.Selection(
        [
            ('ON', 'ON'),
            ('OFF', 'OFF'),
            ('UNKNOWN', 'Unknown'),
        ],
        string='Status',
        default='UNKNOWN',
        readonly=True
    )

    online = fields.Boolean(
        string='Online',
        default=False,
        readonly=True
    )

    last_seen = fields.Datetime(
        string='Last Seen',
        readonly=True
    )

    uptime = fields.Integer(
        string='Uptime (seconds)',
        readonly=True
    )

    api_token = fields.Char(
        string='API Token',
        required=True,
        default='esp32-secret-token'
    )