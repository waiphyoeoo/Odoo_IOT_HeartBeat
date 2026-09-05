from odoo import http, fields
from odoo.http import request


class Esp32Controller(http.Controller):

    @http.route(
        '/iot/esp32/heartbeat',
        type='http',
        auth='public',
        methods=['POST'],
        csrf=False
    )
    def esp32_heartbeat(self, **kwargs):

        data = request.httprequest.get_json(silent=True)

        print("====================================")
        print("ESP32 HEARTBEAT RECEIVED")
        print("Database:", request.db)
        print("Data:", data)
        print("====================================")

        if not data:
            return request.make_json_response(
                {
                    'success': False,
                    'error': 'Invalid JSON'
                },
                status=400
            )

        device_name = data.get('device')
        status = data.get('status')
        ip_address = data.get('ip')
        uptime = data.get('uptime', 0)
        token = data.get('token')

        if not device_name:
            return request.make_json_response(
                {
                    'success': False,
                    'error': 'Device name is required'
                },
                status=400
            )

        device = request.env['esp32.device'].sudo().search(
            [
                ('name', '=', device_name),
                ('api_token', '=', token)
            ],
            limit=1
        )

        if not device:
            return request.make_json_response(
                {
                    'success': False,
                    'error': 'Invalid device or token'
                },
                status=401
            )

        device.sudo().write({
            'status': status if status in ['ON', 'OFF'] else 'UNKNOWN',
            'ip_address': ip_address,
            'online': True,
            'last_seen': fields.Datetime.now(),
            'uptime': int(uptime or 0),
        })

        print("UPDATED DEVICE:", device.name)
        print("STATUS:", device.status)
        print("IP:", device.ip_address)
        print("LAST SEEN:", device.last_seen)
        print("UPTIME:", device.uptime)

        return request.make_json_response({
            'success': True,
            'message': 'Heartbeat received',
            'device': device.name,
            'status': device.status,
        })