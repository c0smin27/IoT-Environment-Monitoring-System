from rest_framework import serializers
from .models import SensorData

# definirea clasei SensorDataSerializer pentru transformarea
# obiectelor modelului SensorData in format JSON si invers
# pentru utilizare in cadrul API REST
class SensorDataSerializer(serializers.ModelSerializer):
    class Meta:
        model = SensorData
        fields = '__all__'
