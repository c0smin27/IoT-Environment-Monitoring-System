from django.urls import path
from . import views
from .views import SensorDataView

urlpatterns = [
    # endpoint REST care primeste datele de la MQTT prin bridge-ul Python
    path('data/', SensorDataView.as_view(), name='sensor-data'),
    # interfata web care afiseaza valorile senzorilor
    path('dashboard/', views.sensor_dashboard, name='sensor_dashboard'),
    # Endpoint AJAX pentru actualizare in timp real in dashboard
    path('get_new_data/', views.get_new_data, name='get_new_data'),
]
