from django.db import models

# model pentru structura datelor primite de la ESP32
class SensorData(models.Model):
    temperature = models.FloatField()
    humidity = models.FloatField()
    voc_index = models.IntegerField()
    timestamp = models.DateTimeField(auto_now_add=True)

    # modul de afisare a obiectelor in shell, admin
    def __str__(self):
        return f"{self.timestamp}: T={self.temperature}°C, H={self.humidity}%, VOC={self.voc_index}"
